
#include "token.h"
#include "print.h"

// closing punctuators like ;, ), ] and } don't count, they never get any operands.
// otherwise, because the primary way to identify a unary operator is that it is preceded by another operator,
// examples like ') * 2' would fail, because it believes that '*' is unary here
// 'operators' include 'statements' (things that have operands, but don't technically return a value).
bool tokenTypeIsOperator(TokenTypeEnum tt) {
    return !((tt == ';')
          || (tt == ')')
          || (tt == ']')
          || (tt == '}')
          || (tt == TT_SYMBOL)
          || (tt == TT_NUMERIC)
          || (tt == TT_STRING));
}

bool tokenTypeIsNullary(TokenTypeEnum tt) {
    switch ((s32) tt) {
        case TT_BREAK:
        case TT_CONTINUE:
            return true;

        default:
            return false;
    }
}

bool tokenTypeIsStatement(TokenTypeEnum tt) {
    if (!tokenTypeIsOperator(tt)) return false;

    // could do tt / 100 == 4 if we move the types that are expression-returning operators,
    // like TT_AND and TT_OR to a different series of token type.
    switch ((s32) tt) {
        case TT_IF:
        case TT_ELSEIF:
        case TT_ELSE:

        case TT_WHILE:
        case TT_DO:
        case TT_FOR:
            return true;

        default:
            return false;
    }
}

bool tokenTypeIsAssignment(TokenTypeEnum tt) {
    switch ((s32) tt) {
        case TT_COLON_EQUALS:
        case '=':
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
        case TT_NOT:
        case '@':
        case '#':
        case '$':
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

        case TT_AND:
        case TT_OR:
        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:

        case '<':
        case TT_LESS_THAN_OR_EQUAL:
        case '>':
        case TT_GREATER_THAN_OR_EQUAL:

        case TT_AS:
        case ':':
        case TT_QUESTION_MARK_COLON:
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
            return 1;

        default:
            return 0;
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
    print("&Token %p | line: %3d, col: %3d, len: %2d, tt: %3d | flags: %d | %s | tk: %s\n",
          (void*) token, token->line, token->column, token->length, token->tt, token->flags, token->filename, token->tk);
}

