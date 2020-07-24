
CC=g++
SRC=./src/
FILES=$(SRC)clue.cpp $(SRC)runtime.cpp $(SRC)lexer.cpp $(SRC)parser.cpp $(SRC)token.cpp $(SRC)node.cpp $(SRC)util.cpp $(SRC)stack.cpp $(SRC)table.cpp $(SRC)symbol.cpp $(SRC)print.cpp $(SRC)reporter.cpp $(SRC)message.cpp $(SRC)string.cpp $(SRC)operator.cpp

FLAGS=-ansi -pedantic -std=c++11 -Wall -g

compile:
		$(CC) $(FILES) $(FLAGS) -o ./bin/clue

clean:
		rm ./bin/clue

