#ifndef HELPER_H_
#define HELPER_H_

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction* inst);

#endif  // HELPER_H_
