#include "./../include/DijSearchState.h"
#include "./../include/helper.h"
#if LLVM_VERSION_MAJOR < 3
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Support/CFG.h"
#else
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#endif
#include <algorithm>
#include <deque>
#include <list>

DijSearchState::DijSearchState(llvm::Instruction *_instruction,
                               uint _distanceFromStart,
                               std::list<llvm::Instruction *> _stack)
    : instruction(getIteratorOnInstruction(_instruction)),
      distanceFromStart(_distanceFromStart), stack() {
  for (std::list<llvm::Instruction *>::iterator it = _stack.begin();
       it != _stack.end(); it++) {
    this->stack.push_back(DijStackEntry(getIteratorOnInstruction(*it)));
  }
}

bool DijSearchState::doesIntroduceRecursion(DijStackEntry next) {
  return std::find(this->stack.begin(), this->stack.end(), next) !=
         this->stack.end();
}
