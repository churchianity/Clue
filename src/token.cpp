

#include "print.h"
#include "token.h"



// closing punctuators like ;, ), ] and } don't count, they never get any operands.
// otherwise, because the primary way to identify a unary operator is that it is preceded by another operator,
// examples like ') * 2' would fail, because it believes that '*' is unary here
bool tokenTypeIsOperator(TokenTypeEnum tt) {
    return !((tt == ';')
          || (tt == ')')
          || (tt == ']')
          || (tt == '}')
          || (tt == TT_SYMBOL)
          || (tt == TT_NUMERIC)
          || (tt == TT_STRING));
}

bool tokenTypeIsStatement(TokenTypeEnum tt) {
    if (!tokenTypeIsOperator(tt)) return false;

    switch ((s32) tt) {
        case TT_IF:
        case TT_ELSE:
            return true;

        default:
            return false;
    }
}

bool tokenTypeIsAssignment(TokenTypeEnum tt) {
    switch ((s32) tt) {
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
        case TT_LOGICAL_AND_EQUALS:
        case TT_LOGICAL_OR_EQUALS:
        case TT_LOGICAL_XOR_EQUALS:

        case TT_INCREMENT: // @NOTE, weird, but technically true, one more reason to remove them
        case TT_DECREMENT:
            return true;

        default: return false;
    }
}

bool tokenTypeIsBitwise(TokenTypeEnum tt) {
    switch ((s32) tt) {
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
s8 tokenTypeUnaryness(TokenTypeEnum tt) {
    switch ((s32) tt) {
        case '+':
        case '-':
            return 2;

        case '~':
        case '!':
        case '@':
        case '#':
        case '$':
        case TT_INCREMENT:
        case TT_DECREMENT:
            return 1;

        default: return 0;
    }
}

s8 tokenTypeBinaryness(TokenTypeEnum tt) {
    switch ((s32) tt) {
        case '(':
            // by far the worst case
            return 0;

        // all of these are sometimes binary, sometimes not
        case '+':
        case '-':
        case '[': //  could be an array literal (unary/operand) or indexer (binary)
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

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:

        case TT_EQUALITY:
        case TT_NOT_EQUALS:

        case '<':
        case TT_LESS_THAN_OR_EQUAL:
        case '>':
        case TT_GREATER_THAN_OR_EQUAL:

        case ':':
        case ',':
        case '.':
            return 1;

        default: return tokenTypeIsAssignment(tt);
    }
}

s8 tokenTypeIsPunctuator(TokenTypeEnum tt) {
    switch ((s32) tt) {
        case ')':
        case ']':
        case '}':
        case ';':
            return 2; // definitely is.

        case '(':
        case '[':
        case '{':
            return 1; // could be. check.

        default:
            return 0; // isn't.
    }
}

const char* tokenTypeToString(TokenTypeEnum tt) {
    switch (tt) {
        case TT_SYMBOL: return "SYMBOL";
        case TT_NUMERIC: return "NUMERIC";
        case TT_STRING: return "STRING";

        default: // these should all be operators @TODO add the value of the enum
            return "OPERATOR";
    }
}

void print(Token* token) {
    print("&Token %p | r: %3d, q: %3d, l: %2d, tt: %3d | flags: %d | %s | tk: %s\n",
          (void*) token, token->line, token->column, token->length, token->tt, token->flags, token->filename, token->tk);
}

