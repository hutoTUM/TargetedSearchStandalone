#ifndef STRATDISTANCE_H_
#define STRATDISTANCE_H_

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"

class StratDistance {
public:
  virtual uint distanceToPass(llvm::Instruction *instr) = 0;
};

class CountInstructions : public StratDistance {
public:
  uint distanceToPass(llvm::Instruction *instr);
};

class CountDecisions : public StratDistance {
public:
  uint distanceToPass(llvm::Instruction *instr);
};

#endif // STRATDISTANCE_H_
