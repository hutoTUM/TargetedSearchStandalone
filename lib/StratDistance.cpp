#include "./../include/StratDistance.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"

uint CountInstructions::distanceToPass(llvm::Instruction * /*instr*/) {
  // Basically everything encrease the distance by one
  return 1;
}

uint CountDecisions::distanceToPass(llvm::Instruction *instr) {
  // Check, if it is a terminator instruction
  if (llvm::isa<llvm::TerminatorInst>(instr)) {
    // Get it as an terminator instruction
    // Direct cast generates memory erros, so we take a view indirections here
    llvm::TerminatorInst *term = instr->getParent()->getTerminator();

    // Check, if have more than one successor
    return (term->getNumSuccessors() > 1) ? 1 : 0;
  }
  return 0;
}
