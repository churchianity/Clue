
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#include "clue.h"

typedef enum TokenTypeEnum {
    TT_SENTINEL,

    TT_SYMBOL,
    TT_OPERATOR,
    TT_NUMERIC,
    TT_STRING
} TokenTypeEnum;

typedef struct Token {
    const char* filename;
    u64 line;
    u64 column;
    u64 length;

    TokenTypeEnum tt;

    /*
    union {
        Identifier* tki;
        unsigned long long tki;
        double float tkf;
        char* tks;
    };
    */

    const char* tk;
    bool bad;

    char* (*toString) (const struct Token*);
} Token;

Token* newToken(const char* filename, u64 line, u64 column, u64 length, TokenTypeEnum tt, const char* tk, bool bad);
char* tokenTypeToString(TokenTypeEnum tt);
void printTokens(Token tokens[]);

#endif

