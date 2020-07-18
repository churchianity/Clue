
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clue.h"
#include "token.h"
#include "util.h"


const char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SENTINEL: return "SENTINEL";

        case TT_SYMBOL: return "SYMBOL";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default:
            return "OPERATOR";
    }
}

Token* newToken(const char* filename, u32 line, u32 col, u32 length, TokenTypeEnum tt, const char* tk, bool bad) {
    Token* token = (Token*) pMalloc(sizeof (Token));

    token->filename = filename;
    token->line = line;
    token->column = col;
    token->length = length;
    token->tt = tt;
    token->tk = tk;
    token->bad = bad;

    return token;
}
