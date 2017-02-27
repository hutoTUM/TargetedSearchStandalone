#ifndef HELPER_H_
#define HELPER_H_

#include <sys/time.h>
#include <stdint.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction* inst);

#endif  // HELPER_H_
