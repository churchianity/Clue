
#include <stdio.h>

#include "token.h"

void printToken(Token token) {
    printf("line: %d, col: %d | tt: %s, tk: %s\n", token.line, token.column, tokenTypeToString(token.tt), token.tk);
}

char* tokenTypeToString(TokenType tt) {
    switch (tt) {
        case TT_BAD_SYMBOL: return "BAD SYMBOL"; break;
        case TT_BAD_OPERATOR: return "BAD OPERATOR"; break;
        case TT_BAD_NUMERIC: return "BAD NUMERIC"; break;
        case TT_BAD_STRING: return "BAD STRING"; break;
        case TT_NO_OP: return "NO_OP"; break;
        case TT_SYMBOL: return "SYMBOL"; break;
        case TT_OPERATOR: return "OPERATOR"; break;
        case TT_NUMERIC: return "NUMERIC"; break;
        case TT_STRING: return "STRING"; break;

        default: return "WTF";
    }
}


