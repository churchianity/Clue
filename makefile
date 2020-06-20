
CC=gcc
FILES=./src/clue.c ./src/runtime.c ./src/lexer.c ./src/parser.c ./src/token.c ./src/node.c ./src/util.c ./src/stack.c

compile:
		$(CC) $(FILES) -o ./bin/clue

clean:
		rm ./bin/clue

