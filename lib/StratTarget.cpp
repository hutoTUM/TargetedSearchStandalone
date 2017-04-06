#include "./../include/StratTarget.h"
#include "./../include/helper.h"

bool FailingAssert::isTheTarget(DijkstraState state) {
  // Check, if it is a call the the assert fail report function
  return isCallToFunction(&*state.instruction, "__assert_fail");
}

bool CallToSpecificFunction::isTheTarget(DijkstraState state) {
  return isCallToFunction(&*state.instruction, this->targetFunctionName);
}

bool EndOfSpecificFunction::isTheTarget(DijkstraState state) {
  // End of a function == the current instruction is
  // (1) the last instruction of a function and
  // (2) is inside the target function
  if (isLastInstructionInFunction(&*state.instruction)) {
    llvm::Function *funcArround = state.instruction->getParent()->getParent();
    return funcArround != NULL && funcArround->getName() == targetFunctionName;
  }
  return false;
}

bool FinalReturn::isTheTarget(DijkstraState state) {
  // Check, if the stack is empty and it is a return instruction
  return state.stack.empty() &&
         isLastInstructionInFunction(&*state.instruction);
}

bool NoTarget::isTheTarget(DijkstraState state) {
  // The searcher never finds its target
  return false;
}
