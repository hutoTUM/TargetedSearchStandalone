#include <iostream>
#include <string>
#include "./../include/DijSearcher.h"
#include "./../include/StratDistance.h"
#include "./../include/StratTarget.h"
#include "./../include/helper.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
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
  auto module = getModuleFromIRFile(BitcodeFilename);
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
    CountInstructions stratDistance{};
    FinalReturn stratTarget{};
    DijSearcher s(&stratDistance, &stratTarget, &(entry->front().front()));

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

    CountDecisions stratDistance{};
    CallToSpecificFunction stratTarget{TargetFunction};
    DijSearcher s(&stratDistance, &stratTarget, &(entry->front().front()));

    llvm::outs() << "Minimal Decisions from " << EntryFunction << " to call of "
                 << TargetFunction << ": " << s.searchForMinimalDistance()
                 << '\n';
  }
}
