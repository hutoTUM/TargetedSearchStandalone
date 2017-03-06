#ifndef INST2RETURNSEARCHER_H_
#define INST2RETURNSEARCHER_H_

#include <stack>
#include <string>
#include "./../DijSearcher.h"

class Inst2ReturnSearcher : public DijSearcher {
 private:
  /**
   * True, iff state is a return instruction with empty stack
   */
  bool isTheTarget(DijSearchState state);

  /**
   * Count all instruction with distance 1
   */
  uint distanceToPass(llvm::Instruction* instr);

 public:
  explicit Inst2ReturnSearcher(llvm::Instruction* start)
      : DijSearcher(start) { /* empty */
  }

  Inst2ReturnSearcher(llvm::Instruction* start,
                      std::list<llvm::Instruction*> stack)
      : DijSearcher(start, stack) { /* empty */
  }
};

#endif  // INST2RETURNSEARCHER_H_
