
CC=g++

SRC=./src/
FILES=$(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)node.cpp $(SRC)reporter.cpp
FLAGS=-ansi -pedantic -g -Wall -std=c++11 -fshort-enums -rdynamic

TEST_SRC=./test/
TESTFILES=$(TEST_SRC)table.cpp
TESTFLAGS=$(FLAGS)

compile:
		clear; $(CC) $(FILES) $(FLAGS) -o ./bin/clue 2>&1 | head -n 40

n:
		$(CC) $(FILES) $(FLAGS) -o ./bin/clue

test:
		$(CC) $(TESTFILES) $(TESTFLAGS) -o ./bin/test/table 2>&1 | head -n 40

e:
		$(CC) -E $(file)

d:
		gdb ./bin/clue

clean:
		rm ./bin/clue
		rm ./bin/test/table

