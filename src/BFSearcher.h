#ifndef BFSEARCHER_H_
#define BFSEARCHER_H_

#include <deque>
#include <stack>
#include "./helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"


/**
 * The entries on the call stack of one state in BFSearcher. They store
 * information about the calling instruction.
 */
class BFStackEntry {
 public:
  explicit BFStackEntry(llvm::BasicBlock::iterator _call)
      : call(_call) { /* empty */
  }
  llvm::BasicBlock::iterator call;
};

/**
 * A class representing the current progress of a single state in BFSearcher.
 */
class BFSearchState {
 public:
  // Iterator on the instruction we are currently looking at
  llvm::BasicBlock::iterator instruction;
  // Accumulated distance from the start of the search till now
  uint distanceFromStart;
  // Stack corresponding to the call path to the current instruction
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

  /**
   * Checks, if the last call on the stack is the start of a recursion
   */
  bool doesIntroduceRecursion();
};

// TODO make some of the public methods private
// TODO reflect this changes also in the inherited classes

/**
 * Breadth first search strategy traversing the whole llvm bitcode looking
 * for the shortest distance to a target. Functions for counting the distance
 * of an instruction and to determine the target are virtual, so this is
 * a base class implementing the strategy for a lot of different search types.
 */
class BFSearcher {
 private:
  // TODO priority queue is even cooler
  std::deque<BFSearchState> searchqueue;

  // Some variables to avoid extreme long search runs
  static const uint maxDistance = 1e5;
  static const uint maxIterations = 1e7;
  static const uint maxQueueLength = 1e4;

 public:
  uint iterationCounter = 0;

  explicit BFSearcher(llvm::Instruction* start);

  /**
   * Runs a search for the minimal distance to the target. If the target is
   * reachable, it returns the distance to it. Otherwise -1 is returned.
   */
  uint searchForMinimalDistance();

  // TODO merge - depends on priority queue
  void appendToSearchQueue(BFSearchState state);

  // TODO merge - depends on priority queue
  void prependToSearchQueue(BFSearchState state);

  /**
   * Small helper function to add a new search state to the search queue. The
   * new distance is calculated internally.
   */
  void enqueueInSearchQueue(BFSearchState oldState,
                            llvm::BasicBlock::iterator next,
                            std::stack<BFStackEntry> newStack);

  /**
   * Get and remove the next entry from the search queue
   */
  BFSearchState popFromSeachQueue();

  /**
   * Do one step in the search. Select the next state and add all its
   * successors to the search queue.
   */
  void doSingleSearchIteration();

  /**
   * Check, if the current state is the target of our search
   */
  virtual bool isTheTarget(BFSearchState state) = 0;

  /**
   * Determine the weight of the current instruction, that is added to the
   * overall distance, when passing this instruction.
   */
  // TODO should return uint not bool - requires switch to priority queue
  virtual bool doesIncrementDistance(llvm::Instruction* instr) = 0;
};

#endif  // BFSEARCHER_H_
