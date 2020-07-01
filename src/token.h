
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef enum TokenTypeEnum {
    TT_SENTINEL,

    TT_SYMBOL,
    TT_OPERATOR,
    TT_NUMERIC,
    TT_STRING
} TokenTypeEnum;

typedef struct Token {
    const char* filename;
    unsigned int line;
    unsigned int column;

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

Token* newToken(const char* filename, unsigned int line, unsigned int column, TokenTypeEnum tt, const char* tk, bool bad);
char* tokenTypeToString(TokenTypeEnum tt);
void printTokens(Token tokens[]);

#endif

