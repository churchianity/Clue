
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
 * @TODO this function is unsafe
 */
static char* toString(const Token* self) {
    const char* tt = tokenTypeToString(self->tt);
    const char* bad = boolToString(self->bad);

    // helper to show something for empty strings
    const char* tk = (strlen(self->tk) == 0) ? "(empty string)" : self->tk;

    const char* format = "%p | line: %d, col: %d | tt: %s, bad: %s, tk: %s\n";

    // magic numbers are assumed lengths as strings of properties after being format specified
    // @TODO fix
    const unsigned int length = 14 + 4 + 4 + strlen(tt) + strlen(bad) + strlen(tk) + strlen(format);

    char* buffer = pmalloc(length + 1);

    snprintf(buffer, length, format
         , self
         , self->line
         , self->column
         , tt
         , bad
         , tk
    );

    return buffer;
}

/**
 * The |tokens| array is assumed to be terminated by a Token of TT_SENTINEL.
 */
void printTokens(Token tokens[]) {
    unsigned int i = 0;

    while (!(tokens[i].tt == TT_SENTINEL)) {
        printf("%s", tokens[i].toString(&tokens[i]));

        ++i;
    }

    printf("%s", tokens[i].toString(&tokens[i])); // sentinel token
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

