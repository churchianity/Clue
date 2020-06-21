
CC=gcc
FILES=./src/clue.c ./src/runtime.c ./src/lexer.c ./src/parser.c ./src/token.c ./src/node.c ./src/util.c ./src/stack.c ./src/table.c
FLAGS=-ansi -std=c99 -pedantic -Wall -g

compile:
		$(CC) $(FILES) $(FLAGS) -o ./bin/clue

clean:
		rm ./bin/clue

