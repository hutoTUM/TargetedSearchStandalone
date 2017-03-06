#include "./../../include/strat/Inst2ReturnSearcher.h"
#include "llvm/IR/Instructions.h"

bool Inst2ReturnSearcher::isTheTarget(DijSearchState state) {
  // Check, if the stack is empty and it is a return instruction
  return state.stack.empty() && llvm::isa<llvm::ReturnInst>(state.instruction);
}

uint Inst2ReturnSearcher::distanceToPass(llvm::Instruction* /*instr*/) {
  // Basically everything encrease the distance by one
  return 1;
}
