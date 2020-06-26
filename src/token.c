
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
 * @TODO this function is probably really unsafe
 */
static char* toString(const Token* self) {
    const char* tt = tokenTypeToString(self->tt);
    const char* bad = boolToString(self->bad);

    // helper to show something for empty strings
    const char* tk = (strlen(self->tk) == 0) ? "__empty_string__" : self->tk;

    const char* format = "%.14p | line: %.4d, col: %.4d | tt: %s, bad: %s, tk: %s\n";

    // pointer length in characters is assumed to be not bigger than 14
    // line # and column # length is truncated to 4 characters
    const unsigned int length = 14 + 4 + 4 + strlen(tt) + strlen(bad) + strlen(tk) + strlen(format);

    char* buffer = pmalloc(sizeof (char) * (length + 1));

    snprintf(buffer, length, format
         , self
         , self->line
         , self->column
         , tokenTypeToString(self->tt)
         , boolToString(self->bad)
         , tk
    );

    return buffer;
}

/**
 *
 */
void printTokens(Token* tokens) {
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

