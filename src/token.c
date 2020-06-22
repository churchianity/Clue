
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "util.h"

char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SENTINEL: return "SENTINEL";

        case TT_SYMBOL: return "SYMBOL";
        case TT_OPERATOR: return "OPERATOR";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default:
            return "UNKNOWN";
    }
}

static void print(const Token* self) {
    // helper to print something for empty strings
    const char* tk = (strlen(self->tk) == 0) ? "__empty_string__" : self->tk;

    printf("line: %d, col: %d | tt: %s, bad: %s, tk: %s\n"
         , self->line
         , self->column
         , tokenTypeToString(self->tt)
         , boolToString(self->bad)
         , tk
    );
}

Token* newToken(unsigned int line, unsigned int col, TokenTypeEnum tt, const char* tk, bool bad) {
    Token* token = pmalloc(sizeof (Token));

    *token = (Token) {
        .line = line,
        .column = col,
        .tt = tt,
        .tk = tk,
        .bad = bad,
        .print = &print
    };

    return token;
}

