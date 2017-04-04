#ifndef HELPER_H_
#define HELPER_H_

#include "llvm/Config/llvm-config.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include <string>

llvm::BasicBlock::iterator getIteratorOnInstruction(llvm::Instruction *inst);

/**
* Checks, whether an instruction is a call to a function with the given name
*/
bool isCallToFunction(llvm::Instruction *inst, llvm::StringRef funcName);

#if LLVM_VERSION_MAJOR > 3 ||                                                  \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 6)
std::unique_ptr<llvm::Module> getModuleFromIRFile(std::string BitcodeFilename);
#else
llvm::Module *getModuleFromIRFile(std::string BitcodeFilename);
#endif

#endif // HELPER_H_
