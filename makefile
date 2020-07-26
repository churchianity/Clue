
CC=g++
SRC=./src/
FILES=$(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)token.cpp $(SRC)node.cpp $(SRC)util.cpp $(SRC)table.cpp $(SRC)symbol.cpp $(SRC)print.cpp $(SRC)reporter.cpp $(SRC)message.cpp $(SRC)string.cpp

FLAGS=-ansi -pedantic -std=c++11 -Wall -Wno-unused-variable -g

compile:
		clear; $(CC) $(FILES) $(FLAGS) -o ./bin/clue 2>&1 | head -n 40

e:
		$(CC) -E $(file)


clean:
		rm ./bin/clue

