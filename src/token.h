
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

typedef enum TokenType {
    TT_BAD_STRING   = -4,
    TT_BAD_NUMERIC  = -3,
    TT_BAD_OPERATOR = -2,
    TT_BAD_SYMBOL   = -1,
    TT_NO_OP        =  0,
    TT_SYMBOL       =  1,
    TT_OPERATOR     =  2,
    TT_NUMERIC      =  3,
    TT_STRING       =  4
} TokenType;

typedef struct {
    char* tk;
    TokenType tt;
    unsigned int line;
    unsigned int column;
} Token;

void printToken(Token token);

char* tokenTypeToString(TokenType tt);

#endif

