#ifndef HELPER_H_
#define HELPER_H_

#include "llvm/Config/llvm-config.h"
#if LLVM_VERSION_MAJOR < 3
#include "llvm/BasicBlock.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#else
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#endif
#include <string>

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction *inst);

/**
* Checks, whether an instruction is a call to a function with the given name
*/
bool isCallToFunction(llvm::Instruction *inst, llvm::StringRef funcName);

/**
* Checks, whether an instruction is the last instruction in its function
*/
bool isLastInstructionInFunction(llvm::Instruction *inst);

#if LLVM_VERSION_MAJOR > 3 ||                                                  \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 6)
std::unique_ptr<llvm::Module> getModuleFromIRFile(std::string BitcodeFilename);
#else
llvm::Module *getModuleFromIRFile(std::string BitcodeFilename);
#endif

#endif // HELPER_H_
