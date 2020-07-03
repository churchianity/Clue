
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen
#include <stdbool.h>

#include "clue.h"
#include "token.h"
#include "util.h"


char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SENTINEL: return "SENTINEL";

        case TT_SYMBOL: return "SYMBOL";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default:
            return "OPERATOR";
    }
}

/**
 * @TODO this function is unsafe and sucks and is stupid
 */
static char* toString(const Token* self) {
    const char* filename = self->filename;
    const char* tt = tokenTypeToString(self->tt);
    const char* bad = boolToString(self->bad);

    // show something for empty strings
    const char* tk = (strlen(self->tk) == 0) ? "(empty string)" : self->tk;

    const char* format = "%p | file: %s, line: %u, col: %u tl: %u | tt: %s, bad: %s, tk: %s\n";

    // magic numbers are (poorly) assumed lengths as strings of properties after being format specified
    // @TODO fix
    const u32 length = 14 + 4 + 4 + 4 + strlen(filename) + strlen(tt) + strlen(bad) + strlen(tk) + strlen(format);

    char* buffer = pMalloc(length + 1);

    snprintf(buffer, length, format
         , self
         , filename
         , self->line
         , self->column
         , self->length
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
    u32 i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        printf("%s", tokens[i].toString(&tokens[i]));

        ++i;
    }

    printf("%s", tokens[i].toString(&tokens[i])); // sentinel token
}

Token* newToken(const char* filename, u32 line, u32 col, u32 length, TokenTypeEnum tt, const char* tk, bool bad) {
    Token* token = pMalloc(sizeof (Token));

    *token = (Token) {
        .filename = filename,
        .line = line,
        .column = col,
        .tt = tt,
        .tk = tk,
        .bad = bad,
        .toString = &toString
    };

    return token;
}

