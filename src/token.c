
#include <stdio.h>

#include "token.h"
#include "util.h"

char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SENTINEL: return "SENTINEL"; break;
        case TT_SYMBOL: return "SYMBOL"; break;
        case TT_OPERATOR: return "OPERATOR"; break;
        case TT_NUMERIC: return "NUMERIC"; break;
        case TT_STRING: return "STRING"; break;

        default: return "WTF";
    }
}

static void print(const Token* self) {
    printf("line: %d, col: %d | tt: %s, tk: %s\n", self->line, self->column, tokenTypeToString(self->tt), self->tk);
}

Token* newToken(unsigned int line, unsigned int col, TokenTypeEnum tt, const char* tk) {
    Token* token = pmalloc(sizeof (Token));

    *token = (Token) {
        .line = line,
        .column = col,
        .tt = tt,
        .tk = tk,
        .print = &print
    };

    return token;
}

