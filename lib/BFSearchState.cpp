#include "./../include/BFSearchState.h"
#include <deque>
#include <list>
#include <stack>
#include "./../include/helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/CFG.h"


BFSearchState::BFSearchState(llvm::Instruction* _instruction,
                             uint _distanceFromStart,
                             std::list<llvm::Instruction*> _stack)
    : instruction(getIteratorOnInstruction(_instruction)),
      distanceFromStart(_distanceFromStart),
      stack() {
  for (std::list<llvm::Instruction*>::iterator it = _stack.begin();
       it != _stack.end(); it++) {
    this->stack.push(BFStackEntry(getIteratorOnInstruction(*it)));
  }
}

bool BFSearchState::doesIntroduceRecursion(BFStackEntry next) {
  // Empty stacks do not contain recursions
  if (this->stack.empty()) {
    return false;
  }

  // Copy the stack
  std::stack<BFStackEntry> stackCopy(this->stack);

  // Extract the function called by the next stack entry
  llvm::CallInst* nextcall = llvm::cast<llvm::CallInst>(next.call);
  llvm::Function* nextcalled = nextcall->getCalledFunction();

  // check if last call in the stack exists twice
  while (!stackCopy.empty()) {
    BFStackEntry probe = stackCopy.top();

    // Extract the function called by the current stack entry
    llvm::CallInst* probecall = llvm::cast<llvm::CallInst>(probe.call);
    llvm::Function* probecalled = probecall->getCalledFunction();

    if (nextcalled == probecalled) {
      return true;
    }
    stackCopy.pop();
  }

  // If no duplicates are found -> no recursion added
  return false;
}
