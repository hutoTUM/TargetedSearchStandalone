#include "llvm/Config/llvm-config.h"
#include <stdio.h>

int main() {
#if LLVM_VERSION_MAJOR
  printf("The clang version is: %d.%d\n", LLVM_VERSION_MAJOR,
         LLVM_VERSION_MINOR);
#else
  printf("The clang version is: undefined\n");
#endif
  return 0;
}
