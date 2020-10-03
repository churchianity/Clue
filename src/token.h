
#ifndef TOKEN_H
#define TOKEN_H

#include "types.h"


enum TokenTypeEnum {
    TT_ANY                      = -1,

    // 0-255 ascii chars here

    // anything that the lexer considers to be a symbol
    TT_SYMBOL                   = 256,

    // anything that the lexer considers to be a number
    TT_NUMERIC                  = 257,

    // anything that the lexer considers to be a string
    TT_STRING                   = 258,

    // TT_OPERATOR              = 259,  // unused (operators have one of the types below);
    // TT_DOUBLE_TILDE          = 260,  // ~~
    // TT_DOUBLE_BACKTICK       = 261,  // ``
    // TT_BOOL_CAST             = 262,  // !!
    // TT_DOUBLE_AT             = 263,  // @@
    // TT_DOUBLE_HASH           = 264,  // ##
    // TT_DOUBLE_DOLLAR_SIGN    = 265,  // $$
    // TT_DOUBLE_PERCENT        = 266,  // %%
    // TT_DOUBLE_HAT            = 267,  // ^^
    TT_LOGICAL_AND              = 268,  // &&
    TT_EXPONENTIATION           = 269,  // **
    // TT_IMPOSSIBLE_1          = 270,  // ((
    // TT_IMPOSSIBLE_2          = 271,  // ))
    // TT_IMPOSSIBLE_3          = 272,  // __
    TT_DECREMENT                = 273,  // --
    TT_INCREMENT                = 274,  // ++
    TT_EQUALITY                 = 275,  // ==
    // TT_DOUBLE_OPEN_BRACE     = 276,  // {{
    // TT_DOUBLE_CLOSE_BRACE    = 277,  // }}
    // TT_DOUBLE_OPEN_BRACKET   = 278,  // [[
    // TT_DOUBLE_CLOSE_BRACKET  = 279,  // ]]
    TT_LOGICAL_OR               = 280,  // ||
    // TT_DOUBLE_BACKSLASH      = 281,  // @NOTE can't write backslashes, makes comment multi-line
    // TT_DOUBLE_COLON          = 282,  // ::
    // TT_IMPOSSIBLE_4          = 283,  // ;;
    // TT_IMPOSSIBLE_5          = 284,  // ""
    // TT_IMPOSSIBLE_6          = 285,  // ''
    TT_LEFT_SHIFT               = 286,  // <<
    // TT_IMPOSSIBLE_7          = 287,  // ,,
    TT_RIGHT_SHIFT              = 288,  // >>
    // TT_DOUBLE_DOT            = 289,  // ..
    // TT_DOUBLE_QMARK          = 290,  // ??
    TT_SINGLE_LINE_COMMENT      = 291,  // //

    // comparison/logical
    TT_NOT_EQUALS               = 292,  // !=
    TT_GREATER_THAN_OR_EQUAL    = 293,  // >=
    TT_LESS_THAN_OR_EQUAL       = 294,  // <=

    TT_COLON_EQUALS             = 299,  // :=

    TT_PLUS_EQUALS              = 300,  // +=
    TT_MINUS_EQUALS             = 301,  // -=
    TT_TIMES_EQUALS             = 302,  // *=
    TT_DIVIDE_EQUALS            = 303,  // /=
    TT_MODULO_EQUALS            = 304,  // %=

    TT_BITWISE_AND_EQUALS       = 305,  // &=
    TT_BITWISE_OR_EQUALS        = 306,  // |=
    TT_BITWISE_XOR_EQUALS       = 307,  // ^=
    // TT_BITWISE_NOT_EQUALS       = 308,  // ~= // doesn't make any sense

    TT_RIGHT_SHIFT_EQUALS       = 309,  // >>=
    TT_LEFT_SHIFT_EQUALS        = 310,  // <<=
    TT_EXPONENTIATION_EQUALS    = 311,  // **=

    TT_IMPORT                   = 400,  // import
    TT_IF                       = 401,  // if
    TT_ELSE                     = 402,  // else
    TT_WHILE                    = 403,  // while
    TT_RETURN                   = 404,  // return

    TT_TYPE_INFO_TAG_NUMBER     = 500,  // Number
    TT_TYPE_INFO_TAG_STRING     = 501,  // String

    // TT_MAKE_IT_BIG              = 0xFFFFFFFFFFFFFFFF
};

enum TokenFlagsEnum {
    TF_BAD          = 1,
    TF_BINARY       = 2,
    // TF_IGNORE       = 4,
    TF_OCTAL        = 8,
    TF_HEXADECIMAL  = 16
};

struct Token {
    const char* filename = null;
    const char* tk       = null;

    u32 line             = -1;
    u32 column           = -1;
    u32 length           = -1;

    TokenTypeEnum tt     = TT_ANY;

    u8 flags = 0;
};

inline bool tokenTypeIsOperator(TokenTypeEnum tt) {
    return !((tt == TT_SYMBOL) || (tt == TT_NUMERIC) || (tt == TT_STRING));
}

inline bool tokenTypeIsAssignment(TokenTypeEnum tt) {
    switch ((int) tt) {
        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:

        case TT_INCREMENT: // @NOTE
        case TT_DECREMENT:
            return true;

        default: return false;
    }
}

inline bool tokenTypeIsBitwise(TokenTypeEnum tt) {
    switch ((int) tt) {
        case '&':
        case '|':
        case '^':
        case '~':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:
            return true;

        default: return false;
    }
}

// these are fuzzy because some operators ('+' and '-') are only sometimes (usually) binary
// and it might be nice to be able to distinguish them, from the ones that can *only* be binary
inline s8 tokenTypeUnaryness(TokenTypeEnum tt) {
    switch ((int) tt) {
        case '+':
        case '-':
            return 2;

        case '~':
        case '!':
        case '@':
        case '$':
            return 1;

        default: return 0;
    }
}

inline s8 tokenTypeBinaryness(TokenTypeEnum tt) {
    switch ((int) tt) {
        case '+':
        case '-':
            return 2;

        case '*':
        case '/':
        case '%':
        case TT_EXPONENTIATION:

        case '&':
        case '|':
        case '^':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:

        case ':':
        case ',':
            return 1;

        default: return tokenTypeIsAssignment(tt);
    }
}

inline const char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SYMBOL: return "SYMBOL";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default: // these should all be operators @TODO add the value of the enum
            return "OPERATOR";
    }
}

#endif

