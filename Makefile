########################################
# Define Variables
########################################
# Some constants for default clang location
LLVM_INCLUDES ?=
LLVM_CONFIG ?= /usr/bin/llvm-config
CLANG ?= /usr/bin/clang

# If a self compiled version of llvm should be used
ifdef LLVM_SRC_PATH
LLVM_BUILD_PATH ?= $(LLVM_SRC_PATH)/Release
LLVM_BIN_PATH ?= $(LLVM_BUILD_PATH)/bin

LLVM_INCLUDES = -I$(LLVM_SRC_PATH)/include -I$(LLVM_BUILD_PATH)/include
LLVM_CONFIG = $(LLVM_BIN_PATH)/llvm-config
CLANG = $(LLVM_BIN_PATH)/clang
endif

# Some more variables for the compiler
CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic
CXXFLAGS_LLVM ?= $(LLVM_INCLUDES)

LLVM_CONFIG_COMMAND = \
		`$(LLVM_CONFIG) --cxxflags --libs` \
		`$(LLVM_CONFIG) --ldflags`

# small helper to build the directory of the current target
MAKETARGETDIR = @mkdir -p $(@D)
# small helper for building binaries
MAKEBINARY = $(CXX) $(CXXFLAGS) $(CXXFLAGS_LLVM) $^ $(LLVM_CONFIG_COMMAND) -o $@

# Find all the source files
CPPS = $(wildcard lib/*.cpp) $(wildcard lib/**/*.cpp)
OBJS = $(addprefix bin/,$(CPPS:.cpp=.o))

# Find all the source files for the tests
TSTCS = $(wildcard test/*.cpp)
TSTOS = $(addprefix bin/,$(TSTCS:.cpp=.o))

# Find all example files
EXMPCS = $(wildcard examples/*.c)
EXMPBC = $(addprefix bin/,$(EXMPCS:.c=.bc))


########################################
# Common aliases
########################################
all: bin/llvm-version bin/decisions bin/notarget examples
examples: $(EXMPBC)


########################################
# The actual build and compiler commands
########################################
bin/%.o: %.cpp
	$(MAKETARGETDIR)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -o $@

bin/%: bin/main/%.o $(OBJS)
	$(MAKEBINARY)

bin/examples/%.bc: examples/%.c
	$(MAKETARGETDIR)
	$(CLANG) -c -emit-llvm -O3 -g $^ -o $@

bin/test/%.o: test/%.cpp
	$(MAKETARGETDIR)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_LLVM) -c $^ $(LLVM_CONFIG_COMMAND) -fexceptions -o $@

bin/testsuite: external/doctest.h $(OBJS) $(TSTOS)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_LLVM) -o $@ $^ $(LLVM_CONFIG_COMMAND) -fexceptions


########################################
# External dependencies
########################################
external/doctest.h:
	# Version 1.1.4
	wget https://raw.githubusercontent.com/onqtam/doctest/d82248081da83c9d609207865bcf3c4ebe64db6c/doctest/doctest.h -O external/doctest.h


########################################
# Miscellaneous
########################################
.PHONY: clean
clean:
	@ rm -rf bin
	@ git checkout bin/.gitignore

.PHONY: test
test: bin/testsuite ./bin/llvm-version examples
	@ ./bin/llvm-version
	./bin/testsuite

.PHONY: valgrind
valgrind: bin/testsuite ./bin/llvm-version examples
	@ ./bin/llvm-version
	valgrind ./bin/testsuite

.PHONY: format
format:
	clang-format -i $(wildcard **/*.cpp) $(wildcard **/*.c) $(wildcard **/*.h)
