#ifndef HELPER_H_
#define HELPER_H_

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction* inst);

/**
* Checks, whether an instruction is a call to a function with the given name
*/
bool isCallToFunction(llvm::Instruction* inst, llvm::StringRef funcName);

#endif  // HELPER_H_
