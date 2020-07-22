
CC=g++
FILES=./src/clue.cpp ./src/runtime.cpp ./src/lexer.cpp ./src/parser.cpp ./src/token.cpp ./src/node.cpp ./src/util.cpp ./src/stack.cpp ./src/table.cpp ./src/symbol.cpp ./src/print.cpp ./src/reporter.cpp ./src/message.cpp ./src/string.cpp
FLAGS=-ansi -pedantic -std=c++11 -Wall -g

compile:
		$(CC) $(FILES) $(FLAGS) -o ./bin/clue

clean:
		rm ./bin/clue

