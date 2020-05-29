
#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType {
    BAD_STRING   = -4,
    BAD_NUMERIC  = -3,
    BAD_OPERATOR = -2,
    BAD_SYMBOL   = -1,
    NO_OP        =  0,
    SYMBOL       =  1,
    OPERATOR     =  2,
    NUMERIC      =  3,
    STRING       =  4
} TokenType;

typedef struct Token {
    char* tk;
    TokenType tt;
    unsigned int line;
    unsigned int column;
} Token;

#endif

