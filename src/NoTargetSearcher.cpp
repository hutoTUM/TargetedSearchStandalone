#include "./NoTargetSearcher.h"
#include "llvm/IR/Instructions.h"

bool NoTargetSearcher::isTheTarget(BFSearchState state) {
  // This searcher never finds his target
  return false;
}

bool NoTargetSearcher::doesIncrementDistance(llvm::Instruction* instr) {
  // Basically everything encrease the distance by one
  return true;
}
