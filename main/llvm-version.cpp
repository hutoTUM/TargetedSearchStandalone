#include "llvm/Config/llvm-config.h"
#include <stdio.h>

int main() {
  printf("The clang version is: %d.%d\n", LLVM_VERSION_MAJOR,
         LLVM_VERSION_MINOR);
  return 0;
}
