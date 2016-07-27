#include "./Decisions2TargetSearcher.h"
#include "llvm/IR/Instructions.h"


bool Decisions2TargetSearcher::isTheTarget(BFSearchState state) {
  // Check, if it is a call instruction
  if (llvm::isa<llvm::CallInst>(state.instruction)) {
    // Check, if it calls our target
    llvm::CallInst* call = llvm::cast<llvm::CallInst>(state.instruction);
    llvm::Function* called = call->getCalledFunction();
    return called->getName().str() == this->targetFunctionName;
  }
  return false;
}

bool Decisions2TargetSearcher::doesIncrementDistance(llvm::Instruction* instr) {
  // Check, if it is a terminator instruction
  if (llvm::isa<llvm::TerminatorInst>(instr)) {
    // Get it as an terminator instruction
    // Direct cast generates memory erros, so we take a view indirections here
    llvm::TerminatorInst* term = instr->getParent()->getTerminator();

    // Check, if have more than one successor
    return (term->getNumSuccessors() > 1);
  }
  return false;
}
