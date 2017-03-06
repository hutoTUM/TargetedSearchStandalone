#ifndef DIJSEARCHSTATE_H_
#define DIJSEARCHSTATE_H_

#include <deque>
#include <list>
#include <queue>
#include <set>
#include <utility>
#include <vector>
#include "./helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"


/**
 * The entries on the call stack of one state in DijSearcher. They store
 * information about the calling instruction.
 */
class DijStackEntry {
 public:
  llvm::BasicBlock::iterator call;
  explicit DijStackEntry(llvm::BasicBlock::iterator _call)
      : call(_call) { /* empty */
  }

  friend bool operator<(const DijStackEntry& l, const DijStackEntry& r) {
    // Very strange way for comparison, but there are no alternatives ...
    return &*l.call < &*r.call;
  }

  friend bool operator==(const DijStackEntry& l, const DijStackEntry& r) {
    // Very strange way for comparison, but there are no alternatives ...
    return &*l.call == &*r.call;
  }
};


/**
 * A class representing the current progress of a single state in DijSearcher.
 */
class DijSearchState {
 public:
  // Iterator on the instruction we are currently looking at
  llvm::BasicBlock::iterator instruction;
  // Accumulated distance from the start of the search till now
  uint distanceFromStart;
  // Stack corresponding to the call path to the current instruction
  std::deque<DijStackEntry> stack;

  DijSearchState(llvm::BasicBlock::iterator _instruction,
                uint _distanceFromStart)
      : instruction(_instruction),
        distanceFromStart(_distanceFromStart) { /* empty */
  }

  DijSearchState(llvm::BasicBlock::iterator _instruction,
                uint _distanceFromStart, std::deque<DijStackEntry> _stack)
      : instruction(_instruction),
        distanceFromStart(_distanceFromStart),
        stack(_stack) { /* empty */
  }

  DijSearchState(llvm::Instruction* _instruction, uint _distanceFromStart,
                std::list<llvm::Instruction*> _stack);

  DijSearchState(llvm::Instruction* _instruction, uint _distanceFromStart)
      : instruction(getIteratorOnInstruction(_instruction)),
        distanceFromStart(_distanceFromStart) { /* empty */
  }

  friend bool operator<(const DijSearchState& l, const DijSearchState& r) {
    return l.distanceFromStart < r.distanceFromStart;
  }

  friend bool operator>(const DijSearchState& l, const DijSearchState& r) {
    return l.distanceFromStart > r.distanceFromStart;
  }

  /**
   * Checks, if next is the start of a recursion
   */
  bool doesIntroduceRecursion(DijStackEntry next);
};

#endif  // DIJSEARCHSTATE_H_
