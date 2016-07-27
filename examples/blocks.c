int oneblock(int n) {
  n += 3;
  n *= 4;
  n >>= 2;
  return n;
}

int fourblocks(int n) {
  if (n >= 0) {
    return n * 4 + 3;
  } else {
    return -n;
  }
}
