
#include "clue.h"
#include "print.h"
#include "string.h"
#include "token.h"
#include "util.h"

bool isOperator(Token* token) {
    return !(token->tt == TT_SYMBOL || token->tt == TT_NUMERIC || token->tt == TT_STRING);
}

const char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SYMBOL: return "SYMBOL";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default:
            return "OPERATOR";
    }
}

/**
 * @FIXME 24 magic number is the longest length for a numeric constant probably but
 * should be a constant defined somewhere else
 */
char* tokenValueToString(Token* token) {
    char* string;

    switch (token->tt) {
        case TT_SYMBOL: string = strcp(token->symbol->name, token->length); break;
        default:        string = strcp(token->op->name, token->length); break;
        case TT_STRING: string = strcp(token->string, token->length); break;
        case TT_NUMERIC:
            string = (char*) pMalloc(sizeof (char) * 24);
            snprintf(string, 24, "%.*lf", token->length, token->number);
            break;
    }

    return string;
}

/**
 * @FIXME
 */
void destroyToken(Token* token) {
    token = null;
}

