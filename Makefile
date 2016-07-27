LLVM_SRC_PATH ?= $$HOME/build/llvm
LLVM_BUILD_PATH ?= $$HOME/build/llvm/Release
LLVM_BIN_PATH ?= $(LLVM_BUILD_PATH)/bin
LLVM_INCLUDES ?= -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include

CXX = g++
CXXFLAGS_LLVM = -O3 -std=c++11 $(LLVM_INCLUDES)

LLVM_CONFIG_COMMAND = \
		`$(LLVM_BIN_PATH)/llvm-config --cxxflags --libs` \
		`$(LLVM_BIN_PATH)/llvm-config --ldflags`

# Find all the source files
CPPS = $(filter-out src/main.cpp,$(wildcard src/*.cpp))
OBJS = $(addprefix bin/,$(notdir $(CPPS:.cpp=.o)))

# Find all the source files for the tests
TSTCS = $(wildcard tests/*.cpp)
TSTOS = $(addprefix bin/tests_,$(notdir $(TSTCS:.cpp=.o)))

# Find all example files
EXACS = $(wildcard examples/*.c)
EXAOS = $(addprefix bin/,$(notdir $(EXACS:.c=.bc)))

all: bin/search

bin/search: $(OBJS) bin/main.o
	$(CXX) $(CXXFLAGS_LLVM) $^ $(LLVM_CONFIG_COMMAND) -o $@

.PHONY: test
test: bin/testsuite $(EXAOS)
	./bin/testsuite

bin/testsuite: external/doctest.h $(OBJS) $(TSTOS)
	$(CXX) $(CXXFLAGS_LLVM) -o $@ $^ $(LLVM_CONFIG_COMMAND) -fexceptions

bin/tests_%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -fexceptions -o $@

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -o $@

bin/%.o: src/%.cpp src/%.h
	$(CXX) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -o $@

bin/%.bc: examples/%.c
	$(LLVM_BIN_PATH)/clang -c -emit-llvm -O3 -g $^ -o $@

external/doctest.h:
	wget https://raw.githubusercontent.com/onqtam/doctest/master/doctest/doctest.h -O external/doctest.h

.PHONY: clean
clean:
	@ rm -rf bin
	@ git checkout bin/.gitignore

.PHONY: format
format:
	clang-format -i $(wildcard **/*.cpp) $(wildcard **/*.c) $(wildcard **/*.h)
