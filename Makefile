########################################
# Define Variables
########################################
LLVM_SRC_PATH ?= $$HOME/build/llvm
LLVM_BUILD_PATH ?= $$HOME/build/llvm/Release
LLVM_BIN_PATH ?= $(LLVM_BUILD_PATH)/bin
LLVM_INCLUDES ?= -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include

CXX ?= g++
CXXFLAGS_LLVM = -std=c++98 $(LLVM_INCLUDES)

LLVM_CONFIG_COMMAND = \
		`$(LLVM_BIN_PATH)/llvm-config --cxxflags --libs` \
		`$(LLVM_BIN_PATH)/llvm-config --ldflags`

# small helper to build the directory of the current target
MAKETARGETDIR = @mkdir -p $(@D)
# small helper for building binaries
MAKEBINARY = $(CXX) $(CXXFLAGS_LLVM) $^ $(LLVM_CONFIG_COMMAND) -o $@

# Find all the source files
#DELETE# MAINS = src/main.cpp src/main-notarget.cpp
CPPS = $(wildcard lib/*.cpp) $(wildcard lib/**/*.cpp)
OBJS = $(addprefix bin/,$(CPPS:.cpp=.o))

# Find all the source files for the tests
TSTCS = $(wildcard tests/*.cpp)
TSTOS = $(addprefix bin/,$(TSTCS:.cpp=.o))

# Find all example files
EXMPCS = $(wildcard examples/*.c)
EXMPBC = $(addprefix bin/,$(EXMPCS:.c=.bc))


########################################
# Common aliases
########################################
all: bin/decisions bin/notarget examples
examples: $(EXMPBC)


########################################
# The actual build and compiler commands
########################################
bin/%.o: %.cpp
	$(MAKETARGETDIR)
	$(CXX) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -o $@

bin/%: bin/main/%.o $(OBJS)
	$(MAKEBINARY)

bin/examples/%.bc: examples/%.c
	$(MAKETARGETDIR)
	$(LLVM_BIN_PATH)/clang -c -emit-llvm -O3 -g $^ -o $@

bin/testsuite: external/doctest.h $(OBJS) $(TSTOS)
	$(CXX) $(CXXFLAGS_LLVM) -o $@ $^ $(LLVM_CONFIG_COMMAND) -fexceptions


########################################
# External dependencies
########################################
external/doctest.h:
	wget https://raw.githubusercontent.com/onqtam/doctest/master/doctest/doctest.h -O external/doctest.h


########################################
# Miscellaneous
########################################
.PHONY: clean
clean:
	@ rm -rf bin
	@ git checkout bin/.gitignore

.PHONY: test
test: bin/testsuite examples
	./bin/testsuite

.PHONY: valgrind
valgrind: bin/testsuite examples
	valgrind ./bin/testsuite

.PHONY: format
format:
	clang-format -i $(wildcard **/*.cpp) $(wildcard **/*.c) $(wildcard **/*.h)
