#include <iostream>
#include <string>
#include "../include/strat/Decisions2TargetCallSearcher.h"
#include "../include/strat/Inst2ReturnSearcher.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include "llvm/Support/CommandLine.h"
#pragma GCC diagnostic pop


llvm::cl::opt<std::string> BitcodeFilename(llvm::cl::Positional,
                                           llvm::cl::desc("<input.bc>"),
                                           llvm::cl::Required);

llvm::cl::opt<std::string> TargetFunction(
    "target", llvm::cl::desc("Name of the targeted Function"),
    llvm::cl::value_desc("targetfunction"));

llvm::cl::opt<std::string> EntryFunction(
    "entry", llvm::cl::desc("Name of the function used as entry point"),
    llvm::cl::init("main"));


int main(int argc, char** argv) {
  // Parse the command line arguments
  llvm::cl::ParseCommandLineOptions(argc, argv);

  // Read the module file
  llvm::SMDiagnostic Err;
  llvm::Module* module =
      llvm::ParseIRFile(BitcodeFilename, Err, llvm::getGlobalContext());
  if (!module) {
    llvm::errs() << "Something is wrong with your bitcode file" << '\n';
    return -1;
  }

  // Get the entry function
  llvm::Function* entry = module->getFunction(EntryFunction);
  if (!entry) {
    llvm::errs() << "Entry function " << EntryFunction << " not found" << '\n';
    return -1;
  }

  if (TargetFunction.empty()) {
    Inst2ReturnSearcher s(&(entry->front().front()));
    llvm::outs() << "Minimal Instruction from " << EntryFunction
                 << " to final return: " << s.searchForMinimalDistance()
                 << '\n';
  } else {
    llvm::Function* target = module->getFunction(TargetFunction);

    if (!target) {
      llvm::errs() << "Target function " << TargetFunction << " not found"
                   << '\n';
      return -1;
    }

    Decisions2TargetCallSearcher s(&(entry->front().front()), TargetFunction);

    llvm::outs() << "Minimal Decisions from " << EntryFunction << " to call of "
                 << TargetFunction << ": " << s.searchForMinimalDistance()
                 << '\n';
  }
}
