
#ifndef TOKEN_H
#define TOKEN_H

#include "scope.h"
#include "types.h"


enum TokenTypeEnum {
    // 0-255 ascii chars here

    // anything that the lexer considers to be a symbol
    TT_SYMBOL                   = 256,

    // anything that the lexer considers to be a number
    TT_NUMERIC                  = 257,

    // anything that the lexer considers to be a string
    TT_STRING                   = 258,

    // TT_OPERATOR              = 259,  // unused (operators have one of the types below);
    // TT_DOUBLE_TILDE          = 260,  // ~~ // bitwise not-not - makes no sense
    // TT_DOUBLE_BACKTICK       = 261,  // `` // this a multi-line comment, never gets parsed
    // TT_BOOL_CAST             = 262,  // !! // no.
    // TT_DOUBLE_AT             = 263,  // @@ // impossible
    // TT_DOUBLE_HASH           = 264,  // ## // @NOTE doesn't mean anything... probably
    // TT_DOUBLE_DOLLAR_SIGN    = 265,  // $$ // double dereference, never gets parsed, just two '$'
    // TT_DOUBLE_PERCENT        = 266,  // %% // @NOTE maybe string format operator if we don't want to overload '%'
    TT_LOGICAL_XOR              = 267, // ^^ // stupid and pointless, but that's not going to stop me
    TT_LOGICAL_AND              = 268, // &&
    TT_EXPONENTIATION           = 269, // **
    // TT_IMPOSSIBLE_1          = 270,  // ((
    // TT_IMPOSSIBLE_2          = 271,  // ))
    // TT_IMPOSSIBLE_3          = 272,  // __
    // TT_DECREMENT                = 273, // -- // @NOTE probably remove these two.
    // TT_INCREMENT                = 274, // ++ // or make it configurable.
    TT_EQUALITY                 = 275, // ==
    // TT_DOUBLE_OPEN_BRACE     = 276,  // {{ // @NOTE probably don't touch these 4
    // TT_DOUBLE_CLOSE_BRACE    = 277,  // }}
    // TT_DOUBLE_OPEN_BRACKET   = 278,  // [[
    // TT_DOUBLE_CLOSE_BRACKET  = 279,  // ]]
    TT_LOGICAL_OR               = 280, // ||
    // TT_DOUBLE_BACKSLASH      = 281,  // can't write backslashes, makes comment multi-line
    // TT_DOUBLE_COLON          = 282,  // :: // no... unless
    // TT_IMPOSSIBLE_4          = 283,  // ;;
    // TT_IMPOSSIBLE_5          = 284,  // ""
    // TT_IMPOSSIBLE_6          = 285,  // ''
    TT_LEFT_SHIFT               = 286, // <<
    // TT_IMPOSSIBLE_7          = 287,  // ,, // no.
    TT_RIGHT_SHIFT              = 288, // >>
    // TT_DOUBLE_DOT            = 289,  // .. // loop range thing? concatenate if we don't want to overload '+'? see complication in numeric case of lexer.
    // TT_DOUBLE_QMARK          = 290,  // ?? // probably not.
    TT_DOUBLE_FORWARD_SLASH     = 291,  // // // floor/integer division?

    // comparison/logical
    TT_NOT_EQUALS               = 292, // !=
    TT_GREATER_THAN_OR_EQUAL    = 293, // >=
    TT_LESS_THAN_OR_EQUAL       = 294, // <=

    TT_QUESTION_MARK_COLON      = 298, // ?:

    TT_COLON_EQUALS             = 299, // :=
    TT_PLUS_EQUALS              = 300, // +=
    TT_MINUS_EQUALS             = 301, // -=
    TT_TIMES_EQUALS             = 302, // *=
    TT_DIVIDE_EQUALS            = 303, // /=
    TT_MODULO_EQUALS            = 304, // %=
    TT_BITWISE_AND_EQUALS       = 305, // &=
    TT_BITWISE_OR_EQUALS        = 306, // |=
    TT_BITWISE_XOR_EQUALS       = 307, // ^=
    TT_RIGHT_SHIFT_EQUALS       = 308, // >>=
    TT_LEFT_SHIFT_EQUALS        = 309, // <<=
    TT_EXPONENTIATION_EQUALS    = 310, // **=

    // begin operators that look like symbols.
    // each token should have an entry in the lexer's keywords table.
    // everything below here is a reserved word, and can't be used as a variable name.
    TT_IMPORT                   = 400, // import

    TT_IF                       = 401, // if
    TT_ELSE                     = 402, // else
    TT_WHILE                    = 403, // while
    TT_RETURN                   = 404, // return
    // TT_MAKE_IT_BIG              = 0xFFFFFFFFFFFFFFFF
};

enum TokenFlagsEnum {
    TF_BAD          = 1,
    TF_BINARY       = 2,
    // TF_IGNORE       = 4,
    TF_OCTAL        = 8,
    TF_HEXADECIMAL  = 16
};

typedef struct Token {
    const char* filename;
    const char* tk;

    Scope* scope;

    u32 line;
    u32 column;
    u32 length;

    TokenTypeEnum tt; // @TODO try and make this be 16 bits.

    u8 flags;
} Token;

bool tokenTypeIsNullary(TokenTypeEnum tt);
bool tokenTypeIsStatement(TokenTypeEnum tt);
bool tokenTypeIsOperator(TokenTypeEnum tt);
bool tokenTypeIsAssignment(TokenTypeEnum tt);
bool tokenTypeIsBitwise(TokenTypeEnum tt);
s8 tokenTypeUnaryness(TokenTypeEnum tt);
s8 tokenTypeBinaryness(TokenTypeEnum tt);
s8 tokenTypeIsPunctuator(TokenTypeEnum tt);
const char* tokenTypeToString(TokenTypeEnum tt);

void print(Token* token);

#endif

