#ifndef NOTARGETSEARCHER_H_
#define NOTARGETSEARCHER_H_

#include <stack>
#include <string>
#include "./../DijSearcher.h"

class NoTargetSearcher : public DijSearcher {
 private:
  // Just a run with more severe limits to reduce test time on infinite loops
  static const uint maxDistance = 10000;
  static const uint maxIterations = 10000;
  static const uint maxQueueLength = 10000;

  /**
   * True, iff state is a return instruction with empty stack
   */
  bool isTheTarget(DijSearchState state);

  /**
   * Count all instruction with distance 1
   */
  uint distanceToPass(llvm::Instruction* instr);

 public:
  explicit NoTargetSearcher(llvm::Instruction* start)
      : DijSearcher(start) { /* empty */
  }

  NoTargetSearcher(llvm::Instruction* start,
                   std::list<llvm::Instruction*> stack)
      : DijSearcher(start, stack) { /* empty */
  }
};

#endif  // NOTARGETSEARCHER_H_
