
CC=clang

SRC=./src/
SRC_FILES=$(SRC)alloc.cpp $(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)node.cpp $(SRC)reporter.cpp $(SRC)string.cpp $(SRC)print.cpp
#COMPILER_FLAGS=-Wc++11-extensions
COMPILER_FLAGS=-ansi -pedantic -g -Wall -std=c++11 -lstdc++ -fshort-enums -rdynamic


compile:
	clear; $(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o ./bin/clue 2>&1 | head -n 40

time:
	time $(CC) $(FILES) $(FLAGS) -o ./bin/clue

clean:
	rm ./bin/clue

