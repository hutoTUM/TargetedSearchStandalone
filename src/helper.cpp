#include "./helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"


llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction* inst) {
  // Construct an iterator for the start instruction
  llvm::BasicBlock* bb = inst->getParent();

  // Skip all instructions before our start instruction
  for (llvm::BasicBlock::iterator II = bb->begin(), IE = bb->end(); II != IE;
       ++II) {
    if (&*II == inst) {
      return II;
    }
  }
  // The instruction was not part of its own block ... this can never happen
  return bb->end();
}
