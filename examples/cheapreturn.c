int catchme(int i);

int cheapreturn(int i) {
  if (i < 0) {
    return i;
  } else {
    if (i << 2 / 3 == 2) {
      catchme(i);
    }
    return 0;
  }
}

int start(int i) {
  i = cheapreturn(i);
  return catchme(i);
}
