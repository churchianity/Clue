
#include "clue.h"
#include "print.h"
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
 * @FIXME
 */
void destroyToken(Token* token) {
    token = null;
}

