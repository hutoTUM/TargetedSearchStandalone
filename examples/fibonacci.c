#include <stdio.h>
#include <stdlib.h>

uint fib(uint n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    return -1;
  } else {
    uint n = atoi(argv[1]);
    printf("fib(%d) = %d\n", n, fib(n));
    return 0;
  }
}
