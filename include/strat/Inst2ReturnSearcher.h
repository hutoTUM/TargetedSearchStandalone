#ifndef INST2RETURNSEARCHER_H_
#define INST2RETURNSEARCHER_H_

#include <stack>
#include <string>
#include "./../BFSearcher.h"

class Inst2ReturnSearcher : public BFSearcher {
 private:
  /**
   * True, iff state is a return instruction with empty stack
   */
  bool isTheTarget(BFSearchState state);

  /**
   * Count all instruction with distance 1
   */
  uint distanceToPass(llvm::Instruction* instr);

 public:
  explicit Inst2ReturnSearcher(llvm::Instruction* start)
      : BFSearcher(start) { /* empty */
  }

  Inst2ReturnSearcher(llvm::Instruction* start,
                      std::list<llvm::Instruction*> stack)
      : BFSearcher(start, stack) { /* empty */
  }
};

#endif  // INST2RETURNSEARCHER_H_
