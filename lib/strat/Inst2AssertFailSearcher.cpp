#include "./../../include/strat/Inst2AssertFailSearcher.h"
#include "./../../include/helper.h"
#include "llvm/IR/Instructions.h"

bool Inst2AssertFailSearcher::isTheTarget(DijSearchState state) {
  // Check, if it is a call the the assert fail report function
  return isCallToFunction(state.instruction, "__assert_fail");
}

uint Inst2AssertFailSearcher::distanceToPass(llvm::Instruction* /*instr*/) {
  // Basically everything encrease the distance by one
  return 1;
}
