#include "../external/doctest.h"
#include "../src/Inst2ReturnSearcher.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

llvm::Instruction* prepareTestCase(std::string filename,
                                   std::string entryfunction) {
  llvm::SMDiagnostic Err;
  llvm::Module* module =
      llvm::ParseIRFile(filename, Err, llvm::getGlobalContext());
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
    Inst2ReturnSearcher s(prepareTestCase("bin/blocks.bc", "oneblock"));
    CHECK(s.searchForMinimalDistance() == 6);
  }

  SUBCASE("Four blocks choose the shortest way") {
    Inst2ReturnSearcher s(prepareTestCase("bin/blocks.bc", "fourblocks"));
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


TEST_CASE(
    "Count the minimal number of instructions to the final return "
    "in bin/callstack.bc") {
  SUBCASE("Function call with no branching") {
    Inst2ReturnSearcher s(
        prepareTestCase("bin/callstack.bc", "noBranchingCall"));
    CHECK(s.searchForMinimalDistance() == 7);
  }

  SUBCASE("Function call with some branching inside") {
    Inst2ReturnSearcher s(
        prepareTestCase("bin/callstack.bc", "oneBranchingCall"));
    CHECK(s.searchForMinimalDistance() == 9);
  }
}

TEST_CASE(
    "Count the minimal number of instructions to the final return "
    "more complex real world test cases") {
  SUBCASE("Simple recursion with fibonacci") {
    Inst2ReturnSearcher s(prepareTestCase("bin/fibonacci.bc", "main"));
    CHECK(s.searchForMinimalDistance() == 5);
  }

  SUBCASE("Simple recursion inside fibonacci") {
    Inst2ReturnSearcher s(prepareTestCase("bin/fibonacci.bc", "fib"));
    CHECK(s.searchForMinimalDistance() == 3);
  }

  SUBCASE("Bigger callstack with divisible") {
    Inst2ReturnSearcher s(prepareTestCase("bin/divisible.bc", "main"));
    CHECK(s.searchForMinimalDistance() == 20);
  }

  SUBCASE("Control Flow Graph cannot be sorted topologically") {
    Inst2ReturnSearcher s(prepareTestCase("bin/doomcircle.bc", "main"));
    CHECK(s.searchForMinimalDistance() == 12);
  }
}
