#ifndef BFSEARCHSTATE_H_
#define BFSEARCHSTATE_H_

#include <deque>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <utility>
#include <vector>
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
  llvm::BasicBlock::iterator call;
  explicit BFStackEntry(llvm::BasicBlock::iterator _call)
      : call(_call) { /* empty */
  }

  friend bool operator<(const BFStackEntry& l, const BFStackEntry& r) {
    // Very strange way for comparison, but there are no alternatives ...
    return &*l.call < &*r.call;
  }
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

  BFSearchState(llvm::Instruction* _instruction, uint _distanceFromStart,
                std::list<llvm::Instruction*> _stack);

  BFSearchState(llvm::Instruction* _instruction, uint _distanceFromStart)
      : instruction(getIteratorOnInstruction(_instruction)),
        distanceFromStart(_distanceFromStart) { /* empty */
  }

  friend bool operator<(const BFSearchState& l, const BFSearchState& r) {
    return l.distanceFromStart < r.distanceFromStart;
  }

  friend bool operator>(const BFSearchState& l, const BFSearchState& r) {
    return l.distanceFromStart > r.distanceFromStart;
  }

  /**
   * Checks, if next is the start of a recursion
   */
  bool doesIntroduceRecursion(BFStackEntry next);
};

#endif  // BFSEARCHSTATE_H_
