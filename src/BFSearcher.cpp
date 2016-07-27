#include <deque>
#include <stack>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"


llvm::BasicBlock::iterator getIteratorFromInstruction(llvm::Instruction* inst) {
  // Construct an iterator for the start instruction
  llvm::BasicBlock* bb = inst->getParent();

  // Skip all instructions before our start instruction
  for (llvm::BasicBlock::iterator II = bb->begin(), IE = bb->end(); II != IE;
       ++II) {
    if (&*II == inst) {
      return II;
    }
  }
  // The instruction was not part of its own block ... this can never happen
  return bb->end();
}

class BFStackEntry {
 public:
  explicit BFStackEntry(llvm::BasicBlock::iterator _call)
      : call(_call) { /* empty */
  }
  llvm::BasicBlock::iterator call;
};

class BFSearchState {
 public:
  llvm::BasicBlock::iterator instruction;
  uint distanceFromStart;
  std::stack<BFStackEntry> stack;

  BFSearchState(llvm::BasicBlock::iterator _instruction,
                uint _distanceFromStart)
      : instruction(_instruction),
        distanceFromStart(_distanceFromStart) { /* empty */
  }
  BFSearchState(llvm::BasicBlock::iterator _instruction,
                uint _distanceFromStart, std::stack<BFStackEntry> _stack)
      : instruction(_instruction),
        distanceFromStart(_distanceFromStart),
        stack(_stack) { /* empty */
  }

  bool doesIntroduceRecursion() {
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
};

class BFSearcher {
 private:
  // TODO priority queue is even cooler
  std::deque<BFSearchState> searchqueue;

  // Some variables to avoid extreme long search runs
  static const uint maxDistance = 1e5;
  static const uint maxIterations = 1e7;
  uint iterationCounter = 0;

 public:
  explicit BFSearcher(llvm::Instruction* start) {
    // Add the start instruction to the search queue with 0 distance so far
    appendToSearchQueue(BFSearchState(getIteratorFromInstruction(start), 0));
  }

  uint SearchForMinimalDistance() {
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

  void appendToSearchQueue(BFSearchState state) {
    // TODO do not add the same state twice
    if (!state.doesIntroduceRecursion()) {
      searchqueue.push_back(state);
    }
  }

  void prependToSearchQueue(BFSearchState state) {
    // TODO do not add the same state twice
    if (!state.doesIntroduceRecursion()) {
      searchqueue.push_front(state);
    }
  }

  void enqueueInSearchQueue(BFSearchState oldState,
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

  BFSearchState popFromSeachQueue() {
    BFSearchState result = searchqueue.front();
    searchqueue.pop_front();
    return result;
  }

  void doSingleSearchIteration() {
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

  virtual bool isTheTarget(BFSearchState state);
  virtual bool doesIncrementDistance(llvm::Instruction* instr);
};


class DecissionsToCallToTargetSeacher : BFSearcher {
 private:
  llvm::Function* target;
  static const uint maxDistance = 1e2;

 public:
  // TODO target function name does not require module access
  DecissionsToCallToTargetSeacher(llvm::Instruction* start,
                                  llvm::Function* _target)
      : BFSearcher(start), target(_target) { /* empty */
  }

  /**
   * True, iff it is a call to our target function
   */
  bool isTheTarget(BFSearchState state) {
    // Check, if it is a call instruction
    if (llvm::isa<llvm::CallInst>(state.instruction)) {
      // Check, if it calls our target
      llvm::CallInst* call = llvm::cast<llvm::CallInst>(state.instruction);
      llvm::Function* called = call->getCalledFunction();
      return called == this->target;
    }
    return false;
  }

  /**
   * Only instructions causing a branching decision increase distance. These
   * instructions are all terminator instructions with more than one successor.
   */
  bool doesIncrementDistance(llvm::Instruction* instr) {
    // Check, if it is a terminator instruction
    if (llvm::isa<llvm::TerminatorInst>(instr)) {
      // Get it as an terminator instruction
      llvm::TerminatorInst* term = llvm::cast<llvm::TerminatorInst>(&instr);
      // Check, if have more than one successor
      return term->getNumSuccessors() > 1;
    }
    return false;
  }
};
