# Generic search for the minimal distance to a target in llvm bitcode

This project provides a stand-alone version of a search algorithm for an arbitrary target in bitcode files.
Given a starting point inside a program, the search finds the shortest path to an instruction qualified as a target using [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm).
Thereby, it is considering the call hierarchy on the stack, as well as avoiding endless loops or infinite recursions.
It provides a small interface, where own definitions of target instructions and distance measures can be implemented.

Possible targets are:
* a call to a specific function
* a failing assert statement
* a specific line of code using debug information inside the bitcode
* (for testing) no target at all to traverse the whole program

Possible distance measures are:
* the number of passed instructions
* the number of decisions taken
* any complex heuristic that you can think of using positive integers

For implementing your own measures/targets, just inherit from the abstract DijSearcher class.

## Build instructions
```
make all test
```
or if you need a local version of LLVM
```
make LLVM_SRC_PATH=<DirOfYourLLVMSrc> all test
```
or if you have followed [our build instruction](https://github.com/tum-i22/klee-install)
```
make LLVM_SRC_PATH=~/build/llvm all test
```
