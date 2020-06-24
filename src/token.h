
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

// perhaps it's valuable to have the 'value' (tk) of the token be either a string OR a double,
// which could be done ahead of time so we don't have to convert in the case of arithmetic.
// this seems slightly more likely to be good than not (we'd still have to deal with converting to string
// in the case of formatted strings or what have you, but that's probably easier and less common)
// but it makes the code a little messier so im avoiding it for now
typedef union {
    char* s;
    double n;
} Tk;

typedef enum TokenTypeEnum {
    TT_SENTINEL,

    TT_SYMBOL,
    TT_OPERATOR,
    TT_NUMERIC,
    TT_STRING
} TokenTypeEnum;

typedef struct Token {
    unsigned int line;
    unsigned int column;
    TokenTypeEnum tt;
    const char* tk;
    bool bad;

    char* (*toString) (const struct Token*);
} Token;

Token* newToken(unsigned int line, unsigned int column, TokenTypeEnum tt, const char* tk, bool bad);
char* tokenTypeToString(TokenTypeEnum tt);

#endif

