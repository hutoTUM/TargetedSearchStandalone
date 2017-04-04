int catchme(int i);

__attribute__((noinline)) int plus42(int i) { return i + 42; }

__attribute__((noinline)) int abs42(int i) {
  if (i < 42) {
    return plus42(i);
  } else {
    return i;
  }
}

int noBranchingCall(int i) {
  i = plus42(i);
  return catchme(i);
}

int oneBranchingCall(int i) {
  i = abs42(i);
  return catchme(i);
}
