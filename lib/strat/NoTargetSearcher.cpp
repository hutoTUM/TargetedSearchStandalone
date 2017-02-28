#include "./../../include/strat/NoTargetSearcher.h"
#include "llvm/IR/Instructions.h"

bool NoTargetSearcher::isTheTarget(BFSearchState /*state*/) {
  // This searcher never finds his target
  return false;
}

uint NoTargetSearcher::distanceToPass(llvm::Instruction* /*instr*/) {
  // Basically everything encrease the distance by one
  return 1;
}
