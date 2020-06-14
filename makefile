#

compile:
		gcc -I ./bin/ ./src/runtime.c ./src/lexer.c ./src/parser.c ./src/token.c ./src/util.c ./src/node.c ./src/stack.c

