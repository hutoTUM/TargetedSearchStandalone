#include "./../include/StratTarget.h"
#include "./../include/helper.h"

bool FailingAssert::isTheTarget(DijkstraState state) {
  // Check, if it is a call the the assert fail report function
  return isCallToFunction(&*state.instruction, "__assert_fail");
}

bool CallToSpecificFunction::isTheTarget(DijkstraState state) {
  return isCallToFunction(&*state.instruction, this->targetFunctionName);
}

bool FinalReturn::isTheTarget(DijkstraState state) {
  // Check, if the stack is empty and it is a return instruction
  return state.stack.empty() && llvm::isa<llvm::ReturnInst>(state.instruction);
}

bool NoTarget::isTheTarget(DijkstraState state) {
  // The searcher never finds its target
  return false;
}
