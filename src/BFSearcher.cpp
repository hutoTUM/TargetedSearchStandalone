#include "./BFSearcher.h"
#include <deque>
#include <iostream>  // TODO remove
#include <stack>
#include "./helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/CFG.h"


bool BFSearchState::doesIntroduceRecursion() {
  // Empty stacks do not contain recursions
  if (this->stack.empty()) {
    return false;
  }

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


BFSearcher::BFSearcher(llvm::Instruction* start) {
  // Add the start instruction to the search queue with 0 distance so far
  appendToSearchQueue(BFSearchState(getIteratorOnInstruction(start), 0));
}

uint BFSearcher::searchForMinimalDistance() {
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
  if (!state.doesIntroduceRecursion() && searchqueue.size() <= maxQueueLength) {
    searchqueue.push_back(state);
  }
}

void BFSearcher::prependToSearchQueue(BFSearchState state) {
  // TODO do not add the same state twice
  if (!state.doesIntroduceRecursion() && searchqueue.size() <= maxQueueLength) {
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

  // nullptr indicates an invalid instruction
  assert(&*curr.instruction != NULL);

  if (llvm::isa<llvm::CallInst>(curr.instruction)) {
    // If call, increase stack and add to search queue

    // Extract the called function
    llvm::CallInst* call = llvm::cast<llvm::CallInst>(curr.instruction);
    llvm::Function* called = call->getCalledFunction();

    // Check if the function is an external call
    if (called && !called->isIntrinsic() && !called->empty()) {
      // It is a call to an defined function

      // Add the current call instruction to the stack
      curr.stack.push(BFStackEntry(curr.instruction));

      // Add everything to the search queue
      enqueueInSearchQueue(curr, called->front().begin(), curr.stack);
    } else {
      // Just skip the called function and treat it as an normal instruction
      enqueueInSearchQueue(curr, (++(curr.instruction))--, curr.stack);
    }

  } else if (llvm::isa<llvm::ReturnInst>(curr.instruction)) {
    // If return, add last entry from stack

    // Check, if we have any point to return to
    if (!curr.stack.empty()) {
      // Extract the top stack frame
      BFStackEntry gobackto = curr.stack.top();
      // and remove it from the stack
      curr.stack.pop();

      // Add everything to the search queue
      enqueueInSearchQueue(curr, ++(gobackto.call), curr.stack);
    }

  } else if (llvm::isa<llvm::TerminatorInst>(curr.instruction)) {
    // If terminal instruction, add all successor

    // Get access to the current block
    llvm::BasicBlock* currblock = curr.instruction->getParent();

    // Iterate over all the successors
    for (llvm::succ_iterator sit = llvm::succ_begin(currblock),
                             et = llvm::succ_end(currblock);
         sit != et; sit++) {
      // And add their first instruction to the search queue
      enqueueInSearchQueue(curr, sit->begin(), curr.stack);
    }

  } else {
    // All other instructions just add their successor
    enqueueInSearchQueue(curr, (++(curr.instruction))--, curr.stack);
  }
}
