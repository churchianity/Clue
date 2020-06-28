
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef union {
    char* s;
    double n;
} Tk;

typedef enum TokenTypeEnum {
    TT_SENTINEL,
    TT_SYMBOL,
    TT_OPERATOR,
    TT_NUMERIC,
    TT_STRING,
    TT_PUNCTUATOR_OPEN,
    TT_PUNCTUATOR_CLOSE
} TokenTypeEnum;

typedef struct Token {
    const char* filename;
    unsigned int line;
    unsigned int column;

    TokenTypeEnum tt;
    const char* tk;
    unsigned int bp;
    bool bad;

    char* (*toString) (const struct Token*);
} Token;

Token* newToken(const char* filename, unsigned int line, unsigned int column, TokenTypeEnum tt, const char* tk, bool bad);
char* tokenTypeToString(TokenTypeEnum tt);
void printTokens(Token tokens[]);

#endif

