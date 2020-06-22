
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef union {
    char* tks;
    double tkm;
} Tok;

typedef enum TokenTypeEnum {
    TT_SENTINEL     =  0,

    TT_SYMBOL       =  1,
    TT_OPERATOR     =  2,
    TT_NUMERIC      =  3,
    TT_STRING       =  4
} TokenTypeEnum;

typedef struct Token {
    unsigned int line;
    unsigned int column;
    TokenTypeEnum tt;
    const char* tk;
    bool bad;

    void (*print) (const struct Token*);
} Token;

Token* newToken(unsigned int line, unsigned int column, TokenTypeEnum tt, const char* tk, bool bad);
char* tokenTypeToString(TokenTypeEnum tt);

#endif

