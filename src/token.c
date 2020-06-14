
#include <stdio.h>

#include "token.h"
#include "util.h"

char* tokenTypeToString(TokenTypeEnum tt) {
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

static void print(const Token* self) {
    printf("line: %d, col: %d | tt: %s, tk: %s\n", self->line, self->column, tokenTypeToString(self->tt), self->tk);
}

Token* newToken(unsigned int line, unsigned int col, TokenTypeEnum tt, const char* tk) {
    Token* token = pmalloc(sizeof (Token));

    if (!token) {
        return NULL;
    }

    *token = (Token) {
        .line = line,
        .column = col,
        .tt = tt,
        .tk = tk,
        .print = &print
    };

    return token;
}

