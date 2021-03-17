
SHELL := /bin/bash

# we are a c++ codebase, but the c++ features we use are relatively minimal compared to the norm
# https://stackoverflow.com/questions/172587/what-is-the-difference-between-g-and-gcc
#CC=clang
CC=gcc
# -ansi -pedantic -Wall		be annoying
# -g 				produce debuging information for use with gdb
# -std=c++(version)		what standard of c++ are we using
# -lstdc++			link the standard library (unfortunately)
#  				new, delete, __cxa_guard_* ?
#
# -lm				link the math library
# -fshort-enums			make enums be as small as possible. not necessarily a good idea
# -rdynamic			export all symbols for debugging with backtrace
#  				eventually, this option should be disabled.
#  				read: https://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html/

COMPILER_FLAGS=-ansi -pedantic -Wall -g -std=c++11 -lstdc++ -lm -rdynamic -fshort-enums 

# not using llvm at the moment.
#LLVM=`llvm-config --cxxflags --ldflags --system-libs --libs core`

CLUE_PATH=./bin/clue
SRC=./src/
SRC_FILES=$(SRC)alloc.cpp $(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)reporter.cpp $(SRC)string.cpp $(SRC)print.cpp $(SRC)token.cpp $(SRC)node.cpp

OUTPUT_FILTER=2>&1 | head -n 40; test $${PIPESTATUS[0]} -eq 0

compile:
	$(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(CLUE_PATH) $(OUTPUT_FILTER)

time:
	time $(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(CLUE_PATH) $(OUTPUT_FILTER)

clean:
	rm $(CLUE_PATH)

