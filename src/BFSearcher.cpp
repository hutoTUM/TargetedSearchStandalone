#include "./BFSearcher.h"
#include <deque>
#include <stack>
#include "./helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

bool BFSearchState::doesIntroduceRecursion() {
  // Copy the stack
  std::stack<BFStackEntry> stackCopy(this->stack);
  BFStackEntry last = stackCopy.top();
  stackCopy.pop();

  // check if last call in the stack exists twice
  while (!stackCopy.empty()) {
    BFStackEntry probe = stackCopy.top();
    // TODO compare called function, not call instruction
    if (last.call == probe.call) {
      return true;
    }
    stackCopy.pop();
  }

  // If no duplicates are found -> no recursion added
  return false;
}

uint BFSearcher::SearchForMinimalDistance() {
  while (!searchqueue.empty() &&
         searchqueue.front().distanceFromStart < maxDistance &&
         iterationCounter < maxIterations) {
    // Check, if we already hit the target
    if (isTheTarget(searchqueue.front())) {
      return searchqueue.front().distanceFromStart;
    }
    doSingleSearchIteration();
    iterationCounter++;
  }
  // Empty search queue and still not found -> target not reachable
  return -1;
}

void BFSearcher::appendToSearchQueue(BFSearchState state) {
  // TODO do not add the same state twice
  if (!state.doesIntroduceRecursion()) {
    searchqueue.push_back(state);
  }
}

void BFSearcher::prependToSearchQueue(BFSearchState state) {
  // TODO do not add the same state twice
  if (!state.doesIntroduceRecursion()) {
    searchqueue.push_front(state);
  }
}

void BFSearcher::enqueueInSearchQueue(BFSearchState oldState,
                                      llvm::BasicBlock::iterator next,
                                      std::stack<BFStackEntry> newStack) {
  bool wasdistanceIncreased = doesIncrementDistance(oldState.instruction);

  if (wasdistanceIncreased) {
    appendToSearchQueue(BFSearchState(
        next, oldState.distanceFromStart + wasdistanceIncreased, newStack));
  } else {
    prependToSearchQueue(BFSearchState(
        next, oldState.distanceFromStart + wasdistanceIncreased, newStack));
  }
}

BFSearchState BFSearcher::popFromSeachQueue() {
  BFSearchState result = searchqueue.front();
  searchqueue.pop_front();
  return result;
}

void BFSearcher::doSingleSearchIteration() {
  // Remove the first state from the queue
  BFSearchState curr = this->popFromSeachQueue();

  if (llvm::isa<llvm::CallInst>(curr.instruction)) {
    // If call, increase stack and add to search queue

    // Add the current call instruction to the stack
    curr.stack.push(BFStackEntry(curr.instruction));

    // Extract the called function
    llvm::CallInst* call = llvm::cast<llvm::CallInst>(curr.instruction);
    llvm::Function* called = call->getCalledFunction();

    // Add everything to the search queue
    enqueueInSearchQueue(curr, called->front().begin(), curr.stack);

  } else if (llvm::isa<llvm::ReturnInst>(curr.instruction)) {
    // If return, add last entry from stack

    // Check, if we have any point to return to
    if (!curr.stack.empty()) {
      // Remove the last call from the stack
      curr.stack.pop();

      // Add everything to the search queue
      enqueueInSearchQueue(curr, ++(curr.stack.top().call), curr.stack);
    }

  } else if (llvm::isa<llvm::TerminatorInst>(curr.instruction)) {
    // If terminal instruction, add all successor

    // Get it as an terminator instruction
    llvm::TerminatorInst* term =
        llvm::cast<llvm::TerminatorInst>(&curr.instruction);

    // Iterate over all the successors
    for (uint i = 0; i < term->getNumSuccessors(); i++) {
      enqueueInSearchQueue(curr, term->getSuccessor(i)->begin(), curr.stack);
    }

  } else {
    // All other instructions just add their successor
    enqueueInSearchQueue(curr, ++(curr.instruction), curr.stack);
  }
}
