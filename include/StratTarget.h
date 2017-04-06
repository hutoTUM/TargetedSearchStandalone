#ifndef STRATTARGET_H_
#define STRATTARGET_H_

#include "./DijkstraState.h"
#include "llvm/ADT/StringRef.h"

class StratTarget {
public:
  virtual bool isTheTarget(DijkstraState state) = 0;
};

class CallToSpecificFunction : public StratTarget {
public:
  llvm::StringRef targetFunctionName;
  explicit CallToSpecificFunction(llvm::StringRef _targetFunctionName)
      : targetFunctionName(_targetFunctionName){/* empty */};
  bool isTheTarget(DijkstraState state);
};

class FailingAssert : public StratTarget {
public:
  bool isTheTarget(DijkstraState state);
};

class FinalReturn : public StratTarget {
public:
  bool isTheTarget(DijkstraState state);
};

class NoTarget : public StratTarget {
public:
  bool isTheTarget(DijkstraState state);
};

#endif // STRATTARGET_H_
