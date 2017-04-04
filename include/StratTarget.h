#ifndef STRATTARGET_H_
#define STRATTARGET_H_

#include "./DijSearchState.h"
#include "llvm/ADT/StringRef.h"

class StratTarget {
public:
  virtual bool isTheTarget(DijSearchState state) = 0;
};

class CallToSpecificFunction : public StratTarget {
public:
  llvm::StringRef targetFunctionName;
  explicit CallToSpecificFunction(llvm::StringRef _targetFunctionName)
      : targetFunctionName(_targetFunctionName){/* empty */};
  bool isTheTarget(DijSearchState state);
};

class FailingAssert : public StratTarget {
public:
  bool isTheTarget(DijSearchState state);
};

class FinalReturn : public StratTarget {
public:
  bool isTheTarget(DijSearchState state);
};

class NoTarget : public StratTarget {
public:
  bool isTheTarget(DijSearchState state);
};

#endif // STRATTARGET_H_
