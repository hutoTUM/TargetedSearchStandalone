#include "./../include/BFSearcher.h"
#include <deque>
#include <list>
#include "./../include/BFSearchState.h"
#include "./../include/helper.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/CFG.h"


BFSearcher::BFSearcher(llvm::Instruction* start) {
  // Add the start instruction to the search queue with 0 distance so far
  addToSearchQueue(BFSearchState(start, 0));

  // Initialize the iteration counter
  this->iterationCounter = 0;
}

BFSearcher::BFSearcher(llvm::Instruction* start,
                       std::list<llvm::Instruction*> stack) {
  // Add the start instruction to the search queue with 0 distance so far
  // and everything that was stored on the stack so far
  addToSearchQueue(BFSearchState(start, 0, stack));

  // Initialize the iteration counter
  this->iterationCounter = 0;
}

uint BFSearcher::searchForMinimalDistance() {
  while (!searchqueue.empty() &&
         searchqueue.top().distanceFromStart < maxDistance &&
         iterationCounter < maxIterations) {
    // Check, if we already hit the target
    if (isTheTarget(searchqueue.top())) {
      return searchqueue.top().distanceFromStart;
    }
    doSingleSearchIteration();
    iterationCounter++;
  }
  // Empty search queue and still not found -> target not reachable
  return -1;
}

void BFSearcher::addToSearchQueue(BFSearchState state) {
  if (!wasAddedEarlier(state) && searchqueue.size() <= maxQueueLength) {
    searchqueue.push(state);
    this->rememberAsAdded(state);
  }
}

void BFSearcher::enqueueInSearchQueue(BFSearchState oldState,
                                      llvm::BasicBlock::iterator next,
                                      std::deque<BFStackEntry> newStack) {
  addToSearchQueue(BFSearchState(
      next, oldState.distanceFromStart + distanceToPass(oldState.instruction),
      newStack));
}

BFSearchState BFSearcher::popFromSeachQueue() {
  BFSearchState result = searchqueue.top();
  searchqueue.pop();
  return result;
}

bool BFSearcher::wasAddedEarlier(BFSearchState state) {
  // Only lookup states, that are the first in their basic block
  if (&(state.instruction->getParent()->front()) == &*state.instruction) {
    return duplicateFilter.count(
        std::make_pair(&*state.instruction, state.stack));
  }
  return false;
}

void BFSearcher::rememberAsAdded(BFSearchState state) {
  // Only store instructions, that are the first in their basic block
  if (&(state.instruction->getParent()->front()) == &*state.instruction) {
    duplicateFilter.insert(std::make_pair(&*state.instruction, state.stack));
  }
}

void BFSearcher::doSingleSearchIteration() {
  // Remove the first state from the queue
  BFSearchState curr = this->popFromSeachQueue();

  // nullptr indicates an invalid instruction
  assert(&*curr.instruction != NULL);

  if (llvm::isa<llvm::CallInst>(curr.instruction)) {
    // If call, increase stack and add to search queue

    llvm::BasicBlock::iterator succ =
        resolveCall(llvm::cast<llvm::CallInst>(curr.instruction));


    // If it is a function, that can acutally be called
    if (succ) {
      // Avoid recursions
      BFStackEntry next(curr.instruction);
      if (!curr.doesIntroduceRecursion(next)) {
        // Add the current call instruction to the stack
        curr.stack.push_back(next);

        // Add everything to the search queue
        enqueueInSearchQueue(curr, succ, curr.stack);
      }
    } else {
      // Just skip the called function and treat it as an normal instruction
      enqueueInSearchQueue(curr, (++(curr.instruction))--, curr.stack);
    }

  } else if (llvm::isa<llvm::ReturnInst>(curr.instruction)) {
    // If return, add last entry from stack

    // Check, if we have any point to return to
    if (!curr.stack.empty()) {
      // Extract the top stack frame
      BFStackEntry gobackto = curr.stack.back();
      // and remove it from the stack
      curr.stack.pop_back();

      // Add everything to the search queue
      enqueueInSearchQueue(curr, ++(gobackto.call), curr.stack);
    }

  } else if (llvm::isa<llvm::TerminatorInst>(curr.instruction)) {
    // If terminal instruction, add all successor

    // Get access to the current block
    llvm::BasicBlock* currblock = curr.instruction->getParent();

    // Iterate over all the successors
    for (llvm::succ_iterator sit = llvm::succ_begin(currblock),
                             et = llvm::succ_end(currblock);
         sit != et; sit++) {
      // And add their first instruction to the search queue
      enqueueInSearchQueue(curr, sit->begin(), curr.stack);
    }

  } else {
    // All other instructions just add their successor
    enqueueInSearchQueue(curr, (++(curr.instruction))--, curr.stack);
  }
}

/**
* Resolves a llvm call operation and returns the first instruction
* executed after the call. If the call cannot be resolved, it returns NULL
*/
llvm::BasicBlock::iterator resolveCall(llvm::CallInst* call) {
  // Extract the called function
  llvm::Function* called = call->getCalledFunction();

  // Check if the function is an external call
  if (called && !called->isIntrinsic() && !called->empty()) {
    // llvm::outs() << "Correctly resolved: " << called->getName() << '\n';
    return called->front().begin();
  }

  // Manually resolve the function pointer to the original main function used
  // in __uClibc_main of KLEE's uClibc implementation
  if (llvm::isa<llvm::LoadInst>(call->getCalledValue()) &&
      call->getParent()->getParent()->getName() == "__uClibc_main") {
    // Extract the type of the function allocated in the called pointer
    llvm::LoadInst* load = llvm::cast<llvm::LoadInst>(call->getCalledValue());
    llvm::AllocaInst* alloca =
        llvm::cast<llvm::AllocaInst>(load->getPointerOperand());
    llvm::PointerType* inptr =
        llvm::cast<llvm::PointerType>(alloca->getAllocatedType());
    llvm::FunctionType* infnc =
        llvm::cast<llvm::FunctionType>(inptr->getElementType());

    // Check, if the inner function has the signature of a main function
    if (infnc->getNumParams() == 3 && infnc->getReturnType()->isIntegerTy() &&
        infnc->getParamType(0)->isIntegerTy() &&
        infnc->getParamType(1)->isPointerTy() &&
        infnc->getParamType(2)->isPointerTy()) {
      // Extract actual content of the parameter for __uClibc_main

      llvm::CallInst* call2uclibc =
          llvm::cast<llvm::CallInst>(call->getParent()
                                         ->getParent()
                                         ->getParent()
                                         ->getFunction("main")
                                         ->getEntryBlock()
                                         .begin());
      if (call2uclibc->getNumArgOperands() == 7) {
        llvm::Value* probe = call2uclibc->getArgOperand(0)->stripPointerCasts();
        if (llvm::isa<llvm::Function>(probe)) {
          // And return a jump to the first instruction of the user main
          llvm::Function* usermain = llvm::cast<llvm::Function>(probe);
          return usermain->front().begin();
        }
      }
    }
  }

  return NULL;
}
