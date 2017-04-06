#include "../external/doctest.h"
#include "./../include/Dijkstra.h"
#include "./../include/StratDistance.h"
#include "./../include/StratTarget.h"
#include "./../include/helper.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

llvm::Instruction *getEntryPoint(llvm::Module *module,
                                 llvm::StringRef entryfunction = "main") {
  llvm::Function *function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction *start = &(function->front().front());
  REQUIRE(start);

  return start;
}

llvm::Instruction *getEntryPoint(std::unique_ptr<llvm::Module> &module,
                                 llvm::StringRef entryfunction = "main") {
  return getEntryPoint(module.get(), entryfunction);
}

TEST_CASE("Count the minimal number of instructions to the final return "
          "in bin/examples/blocks.bc") {
  auto module = getModuleFromIRFile("bin/examples/blocks.bc");
  REQUIRE(module);

  CountInstructions stratDistance{};
  FinalReturn stratTarget{};

  SUBCASE("One block is passed completely") {
    Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module, "oneblock"));
    CHECK(s.searchForMinimalDistance() == 6);
  }

  SUBCASE("Four blocks choose the shortest way") {
    Dijkstra s(&stratDistance, &stratTarget,
               getEntryPoint(module, "fourblocks"));
    CHECK(s.searchForMinimalDistance() == 6);
  }

  SUBCASE("Target instruction has distance 0") {
    // Read the last instruction of the basic block
    llvm::Function *function = module->getFunction("oneblock");
    REQUIRE(function);
    llvm::Instruction *last = &(function->front().back());
    REQUIRE(last);

    Dijkstra s(&stratDistance, &stratTarget, last);
    CHECK(s.searchForMinimalDistance() == 0);
  }
}

TEST_CASE("Count the minimal number of instructions to the final return "
          "in bin/examples/callstack.bc") {
  auto module = getModuleFromIRFile("bin/examples/callstack.bc");
  REQUIRE(module);

  CountInstructions stratDistance{};
  FinalReturn stratTarget{};

  SUBCASE("Function call with no branching") {
    Dijkstra s(&stratDistance, &stratTarget,
               getEntryPoint(module, "noBranchingCall"));
    CHECK(s.searchForMinimalDistance() == 7);
  }

  SUBCASE("Function call with some branching inside") {
    Dijkstra s(&stratDistance, &stratTarget,
               getEntryPoint(module, "oneBranchingCall"));
    CHECK(s.searchForMinimalDistance() == 9);
  }
}

TEST_CASE("Count the minimal number of instructions to the final return "
          "more complex real world test cases") {
  CountInstructions stratDistance{};
  FinalReturn stratTarget{};

  SUBCASE("On bin/examples/fibonacci.bc") {
    auto module = getModuleFromIRFile("bin/examples/fibonacci.bc");
    REQUIRE(module);

    SUBCASE("Simple recursion with fibonacci") {
      Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module));
      CHECK(s.searchForMinimalDistance() == 5);
    }

    SUBCASE("Simple recursion inside fibonacci") {
      Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module, "fib"));
      CHECK(s.searchForMinimalDistance() == 3);
    }
  }

  SUBCASE("Bigger callstack with divisible") {
    auto module = getModuleFromIRFile("bin/examples/divisible.bc");
    Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module));
    CHECK(s.searchForMinimalDistance() == 20);
  }

  SUBCASE("Control Flow Graph cannot be sorted topologically") {
    auto module = getModuleFromIRFile("bin/examples/doomcircle.bc");
    Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module));
    CHECK(s.searchForMinimalDistance() == 12);
  }
}

TEST_CASE("Count the minimal number of instructions to assert failure "
          "in bin/assert.bc") {
  auto module = getModuleFromIRFile("bin/examples/assert.bc");
  REQUIRE(module);

  CountInstructions stratDistance{};
  FailingAssert stratTarget{};
  Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module));

  CHECK(s.searchForMinimalDistance() == 4);
}

TEST_CASE("Count the minimal number of instructions to assert failure "
          "in bin/assert.bc") {
  auto module = getModuleFromIRFile("bin/examples/branches.bc");
  REQUIRE(module);

  CountInstructions stratDistance{};
  EndOfSpecificFunction stratTarget{"quarter"};
  Dijkstra s(&stratDistance, &stratTarget, getEntryPoint(module, "quarter"));

  CHECK(s.searchForMinimalDistance() == 4);
}
