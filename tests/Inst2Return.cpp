#include "../external/doctest.h"
#include "../src/Inst2ReturnSearcher.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

llvm::Instruction* prepareTestCase(std::string entryfunction) {
  llvm::SMDiagnostic Err;
  llvm::Module* module =
      llvm::ParseIRFile("bin/blocks.bc", Err, llvm::getGlobalContext());
  REQUIRE(module);

  llvm::Function* function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction* start = &(function->front().front());
  REQUIRE(start);

  return start;
}


TEST_CASE(
    "Count the minimal number of instructions to the final return "
    "in bin/blocks.bc") {
  SUBCASE("One block is passed completely") {
    Inst2ReturnSearcher s(prepareTestCase("oneblock"));
    CHECK(s.searchForMinimalDistance() == 6);
  }

  SUBCASE("Four blocks choose the shortest way") {
    Inst2ReturnSearcher s(prepareTestCase("fourblocks"));
    CHECK(s.searchForMinimalDistance() == 6);
  }

  SUBCASE("Target instruction has distance 0") {
    llvm::SMDiagnostic Err;
    llvm::Module* module =
        llvm::ParseIRFile("bin/blocks.bc", Err, llvm::getGlobalContext());
    REQUIRE(module);

    llvm::Function* function = module->getFunction("oneblock");
    REQUIRE(function);

    llvm::Instruction* last = &(function->front().back());
    REQUIRE(last);

    Inst2ReturnSearcher l(last);
    CHECK(l.searchForMinimalDistance() == 0);
  }
}
