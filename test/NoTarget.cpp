#include "../external/doctest.h"
#include "../src/NoTargetSearcher.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

uint getIterationsInSearch(std::string filename, std::string entryfunction) {
  llvm::SMDiagnostic Err;
  llvm::Module* module =
      llvm::ParseIRFile(filename, Err, llvm::getGlobalContext());
  REQUIRE(module);

  llvm::Function* function = module->getFunction(entryfunction);
  REQUIRE(function);

  llvm::Instruction* start = &(function->front().front());
  REQUIRE(start);

  NoTargetSearcher s(start);

  s.searchForMinimalDistance();

  return s.iterationCounter;
}


TEST_CASE("Check, if the search hung up in infinite queues") {
  // the limits might be a little generous, but loops break them anyway

  SUBCASE("Recursion in fibonacci numbers") {
    CHECK(getIterationsInSearch("bin/fibonacci.bc", "main") < 50);
  }

  SUBCASE("while(true) continue") {
    CHECK(getIterationsInSearch("bin/infinite.bc", "infinite") < 10);
  }
}
