#include "./../external/doctest.h"
#include "./../include/Dijkstra.h"
#include "./../include/StratDistance.h"
#include "./../include/StratTarget.h"
#include "./../include/helper.h"
#include "llvm/ADT/StringRef.h"
#if LLVM_VERSION_MAJOR < 3
#include "llvm/Function.h"
#include "llvm/Instruction.h"
#include "llvm/Module.h"
#else
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#endif

uint executeSearchRun(llvm::StringRef filename, llvm::StringRef entryfunction,
                      llvm::StringRef targetfunction) {
#if LLVM_VERSION_MAJOR > 3 ||                                                  \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 6)
  std::unique_ptr<llvm::Module> module;
#else
  llvm::Module *module;
#endif
  module = getModuleFromIRFile(filename);
  REQUIRE(module);

  llvm::Function *function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction *start = &(function->front().front());
  REQUIRE(start);

  CountDecisions stratDistance;
  CallToSpecificFunction stratTarget(targetfunction);
  Dijkstra s(&stratDistance, &stratTarget, start);

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
    CHECK(executeSearchRun("bin/examples/branches.bc", "twice", "catchme") ==
          1);
  }

  SUBCASE("Two br decisions on the way to target") {
    CHECK(executeSearchRun("bin/examples/branches.bc", "quarter", "catchme") ==
          2);
  }
}

TEST_CASE("Detects, if the target is not reachable at all") {
  SUBCASE("One block with no call to target function") {
    CHECK(executeSearchRun("bin/examples/nocall.bc", "main", "catchme") == -1);
  }

  SUBCASE("Lower function cannot reach higher functions") {
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby2", "divby5") ==
          -1);
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby6", "divby10") ==
          -1);
    CHECK(executeSearchRun("bin/examples/divisible.bc", "divby30", "main") ==
          -1);
  }
}

TEST_CASE("Complicated pathes") {
  SUBCASE("Return is better than directly hit the target") {
    CHECK(executeSearchRun("bin/examples/cheapreturn.bc", "start", "catchme") ==
          1);
  }
}
