#include "../external/doctest.h"
#include "../include/strat/Decisions2TargetCallSearcher.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

uint executeSearchRun(std::string filename, std::string entryfunction,
                      std::string targetfunction) {
  llvm::SMDiagnostic Err;
  llvm::Module* module =
      llvm::ParseIRFile(filename, Err, llvm::getGlobalContext());
  REQUIRE(module);

  llvm::Function* function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction* start = &(function->front().front());
  REQUIRE(start);

  Decisions2TargetCallSearcher s(start, targetfunction);
  return s.searchForMinimalDistance();
}


TEST_CASE("Count the minimal number of decisions to target function call") {
  SUBCASE("Target function is in the same block") {
    CHECK(executeSearchRun("bin/examples/flat.bc", "flat", "catchme") == 0);
  }

  SUBCASE("Target function is in another function") {
    CHECK(executeSearchRun("bin/examples/flat.bc", "main", "catchme") == 0);
  }

  SUBCASE("One br decision on the way to target") {
    CHECK(executeSearchRun("bin/examples/branches.bc", "twice", "catchme") == 1);
  }

  SUBCASE("Two br decisions on the way to target") {
    CHECK(executeSearchRun("bin/examples/branches.bc", "quarter", "catchme") == 2);
  }
}

TEST_CASE("Detects, if the target is not reachable at all") {
  SUBCASE("One block with no call to target function") {
    CHECK(executeSearchRun("bin/examples/nocall.bc", "main", "catchme") == -1);
  }

  SUBCASE("Lower function cannot reach higher functions") {
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby2", "divby5") == -1);
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby6", "divby10") == -1);
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby30", "main") == -1);
  }
}

TEST_CASE("Complicated pathes") {
  SUBCASE("Return is better than directly hit the target") {
    CHECK(executeSearchRun("bin/examples/cheapreturn.bc", "start", "catchme") == 1);
  }
}
