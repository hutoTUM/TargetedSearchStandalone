#include "./../external/doctest.h"
#include "./../include/DijSearcher.h"
#include "./../include/StratDistance.h"
#include "./../include/StratTarget.h"
#include "./../include/helper.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"

uint getIterationsInSearch(llvm::StringRef filename,
                           llvm::StringRef entryfunction) {
  auto module = getModuleFromIRFile(filename);
  REQUIRE(module);

  llvm::Function *function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction *start = &(function->front().front());
  REQUIRE(start);

  CountInstructions stratDistance{};
  NoTarget stratTarget{};
  DijSearcher s(&stratDistance, &stratTarget, start);

  s.searchForMinimalDistance();

  return s.iterationCounter;
}

TEST_CASE("Check, if the search hung up in infinite queues") {
  // the limits might be a little generous, but loops break them anyway

  SUBCASE("Recursion in fibonacci numbers") {
    CHECK(getIterationsInSearch("bin/examples/fibonacci.bc", "main") < 60);
  }

  SUBCASE("while(true) continue") {
    CHECK(getIterationsInSearch("bin/examples/infinite.bc", "infinite") < 10);
  }
}
