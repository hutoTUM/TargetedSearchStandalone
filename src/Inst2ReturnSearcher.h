#ifndef INST2RETURNSEARCHER_H_
#define INST2RETURNSEARCHER_H_

#include <stack>
#include <string>
#include "./BFSearcher.h"

class Inst2ReturnSearcher : public BFSearcher {
 public:
  explicit Inst2ReturnSearcher(llvm::Instruction* start)
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

#endif  // INST2RETURNSEARCHER_H_