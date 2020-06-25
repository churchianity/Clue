
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

/**
 * @TODO this function is reeaaally stupid
 */
static char* toString(const Token* self) {
    // helper to show something for empty strings
    const char* tk = (strlen(self->tk) == 0) ? "__empty_string__" : self->tk;
    const char* format = "line: %d, col: %d | tt: %s, bad: %s, tk: %s\n";
    char* buffer = pmalloc(260);

    snprintf(buffer, 240, format
         , self->line
         , self->column
         , tokenTypeToString(self->tt)
         , boolToString(self->bad)
         , tk
    );

    return buffer;
}

Token* newToken(unsigned int line, unsigned int col, TokenTypeEnum tt, const char* tk, bool bad) {
    Token* token = pmalloc(sizeof (Token));

    *token = (Token) {
        .line = line,
        .column = col,
        .tt = tt,
        .tk = tk,
        .bad = bad,
        .toString = &toString
    };

    return token;
}

