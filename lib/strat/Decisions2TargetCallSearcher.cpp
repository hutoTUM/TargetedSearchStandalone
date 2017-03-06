#include "./../../include/strat/Decisions2TargetCallSearcher.h"
#include "./../../include/helper.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"


bool Decisions2TargetCallSearcher::isTheTarget(DijSearchState state) {
  return isCallToFunction(state.instruction, this->targetFunctionName);
}

uint Decisions2TargetCallSearcher::distanceToPass(llvm::Instruction* instr) {
  // Check, if it is a terminator instruction
  if (llvm::isa<llvm::TerminatorInst>(instr)) {
    // Get it as an terminator instruction
    // Direct cast generates memory erros, so we take a view indirections here
    llvm::TerminatorInst* term = instr->getParent()->getTerminator();

    // Check, if have more than one successor
    return (term->getNumSuccessors() > 1) ? 1 : 0;
  }
  return 0;
}
