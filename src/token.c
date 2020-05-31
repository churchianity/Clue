
#include <stdio.h>

#include "token.h"

void printToken(Token token) {
    printf("line: %d, col: %d | tt: %d, tk: %s\n", token.line, token.column, token.tt, token.tk);
}



