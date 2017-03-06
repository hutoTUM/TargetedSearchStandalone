#ifndef DECISIONS2TARGETCALLSEARCHER_H_
#define DECISIONS2TARGETCALLSEARCHER_H_

#include "llvm/ADT/StringRef.h"
#include "./../DijSearcher.h"

class Decisions2TargetCallSearcher : public DijSearcher {
 private:
  llvm::StringRef targetFunctionName;
  static const uint maxDistance = 100;

  /**
   * True, iff it is a call to our target function
   */
  bool isTheTarget(DijSearchState state);

  /**
   * Only instructions causing a branching decision increase distance by 1.
   * These instructions are all terminators with more than one successor.
   */
  uint distanceToPass(llvm::Instruction* instr);

 public:
  Decisions2TargetCallSearcher(llvm::Instruction* start,
                               llvm::StringRef _targetFunctionName)
      : DijSearcher(start), targetFunctionName(_targetFunctionName) { /* empty */
  }

  Decisions2TargetCallSearcher(llvm::Instruction* start,
                               std::list<llvm::Instruction*> stack,
                               llvm::StringRef _targetFunctionName)
      : DijSearcher(start, stack),
        targetFunctionName(_targetFunctionName) { /* empty */
  }
};

#endif  // DECISIONS2TARGETCALLSEARCHER_H_
