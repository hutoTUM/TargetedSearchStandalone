#ifndef HELPER_H_
#define HELPER_H_

#include <sys/time.h>
#include <stdint.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction* inst);

static inline uint64_t getCurrentTimeInMicroSeconds() {
  struct timeval time;
  gettimeofday(&time, NULL);

  // microseconds = 1 000 000 * seconds = 1000 * milliseconds
  return time.tv_sec * 1000000 + time.tv_usec;
}

static inline uint64_t getCurrentTimeInMilliSeconds() {
  // milliseconds = microseconds / 1000
  // 0.5 for the rounding logic
  return (getCurrentTimeInMicroSeconds() / 1000.0 + 0.5);
}


#endif  // HELPER_H_
