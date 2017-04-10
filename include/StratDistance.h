#ifndef STRATDISTANCE_H_
#define STRATDISTANCE_H_

#include "llvm/Config/llvm-config.h"
#if LLVM_VERSION_MAJOR < 3
#include "llvm/BasicBlock.h"
#include "llvm/InstrTypes.h"
#else
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#endif

class StratDistance {
public:
  virtual ~StratDistance(){/* empty */};
  virtual uint distanceToPass(llvm::Instruction *instr) = 0;
};

class CountInstructions : public StratDistance {
public:
  ~CountInstructions();
  uint distanceToPass(llvm::Instruction *instr);
};

class CountDecisions : public StratDistance {
public:
  ~CountDecisions();
  uint distanceToPass(llvm::Instruction *instr);
};

#endif // STRATDISTANCE_H_
