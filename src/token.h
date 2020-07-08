
#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#include "clue.h"
#include "symbol.h"


// this should be a complete list of all the terminals in the language
typedef enum TokenTypeEnum {
    TT_SENTINEL = -256,

    // ascii chars here

    TT_SYMBOL = 256,
    TT_NUMERIC = 257,
    TT_STRING = 258,

    TT_COLON_ASSIGNMENT = 259,
    TT_PLUS_ASSIGNMENT = 260,
    TT_MINUS_ASSIGNMENT = 261,
    TT_TIMES_ASSIGNMENT = 262,
    TT_DIVIDE_SLASH_ASSIGNMENT = 263,
    TT_MODULO_ASSIGNMENT = 264,

    TT_BITWISE_AND_ASSIGNMENT = 265,
    TT_BITWISE_OR_ASSIGNMENT = 266,
    TT_BITWISE_XOR_ASSIGNMENT = 267,

    TT_LEFT_SHIFT_ASSIGNMENT = 268,
    TT_RIGHT_SHIFT_ASSIGNMENT = 269,

    TT_LOGICAL_AND = 270,
    TT_LOGICAL_OR = 271,
    TT_LOGICAL_XOR = 272,

    TT_EQUALS = 273,
    TT_NOT_EQUALS = 274,

    TT_GREATER_THAN_OR_EQUAL = 275,
    TT_LESS_THAN_OR_EQUAL = 276
} TokenTypeEnum;

typedef struct Token {
    const char* filename;
    u32 line;
    u32 column;
    u32 length;

    TokenTypeEnum tt;
    const char* tk; // the actual token value as a string - will be converted in the case of symbols and numeric types later

    bool bad; // the lexer can set a flag that lets you know this token is invalid regardless of context which is useful later
} Token;

Token* newToken(const char* filename, u32 line, u32 column, u32 length, TokenTypeEnum tt, const char* tk, bool bad);
const char* tokenTypeToString(TokenTypeEnum tt);

#endif

