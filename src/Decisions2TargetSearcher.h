#ifndef DECISIONS2TARGETSEARCHER_H_
#define DECISIONS2TARGETSEARCHER_H_

#include <string>
#include "./BFSearcher.h"

class Decisions2TargetSearcher : public BFSearcher {
 private:
  std::string targetFunctionName;
  static const uint maxDistance = 1e2;

  /**
   * True, iff it is a call to our target function
   */
  bool isTheTarget(BFSearchState state);

  /**
   * Only instructions causing a branching decision increase distance. These
   * instructions are all terminator instructions with more than one successor.
   */
  bool doesIncrementDistance(llvm::Instruction* instr);

 public:
  Decisions2TargetSearcher(llvm::Instruction* start,
                           std::string _targetFunctionName)
      : BFSearcher(start), targetFunctionName(_targetFunctionName) { /* empty */
  }
};

#endif  // DECISIONS2TARGETSEARCHER_H_
