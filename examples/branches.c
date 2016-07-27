int catchme(int i);

int twice(int n) {
  if (n >= 0) {
    return catchme(n);
  } else {
    return -n;
  }
}

int quarter(int i) {
  if (i >= 0) {
    if (i % 2 == 0) {
      return 0;
    } else {
      return catchme(1);
    }
  } else {
    return -1;
  }
}
