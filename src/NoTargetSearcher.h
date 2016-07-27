#ifndef NOTARGETSEARCHER_H_
#define NOTARGETSEARCHER_H_

#include <stack>
#include <string>
#include "./BFSearcher.h"

class NoTargetSearcher : public BFSearcher {
 private:
  // Just a run with more severe limits to reduce test time on infinite loops
  static const uint maxDistance = 1e4;
  static const uint maxIterations = 1e4;
  static const uint maxQueueLength = 1e4;

 public:
  explicit NoTargetSearcher(llvm::Instruction* start)
      : BFSearcher(start) { /* empty */
  }

  /**
   * True, iff state is a return instruction with empty stack
   */
  bool isTheTarget(BFSearchState state);

  /**
   * Count all instruction with distance 1
   */
  bool doesIncrementDistance(llvm::Instruction* instr);
};

#endif  // NOTARGETSEARCHER_H_
