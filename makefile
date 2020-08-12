
CC=g++

SRC=./src/
FILES=$(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)token.cpp $(SRC)node.cpp $(SRC)util.cpp $(SRC)symbol.cpp $(SRC)print.cpp $(SRC)reporter.cpp $(SRC)operator.cpp
FLAGS=-ansi -pedantic -std=c++11 -Wall -Wno-unused-variable -fshort-enums -g

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

clean:
		rm ./bin/clue
		rm ./bin/test/table

