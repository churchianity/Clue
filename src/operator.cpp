
#include "table.h"
#include "token.h"
#include "operator.h"
#include "util.h"


static Table* operatorTable = newTable(50);


void initOperatorTable() {
    Table* t = operatorTable;

    t->insert(t, ":",   NULL);

    t->insert(t, ";",   NULL);
    t->insert(t, ",",   NULL);
    t->insert(t, "{",   NULL);
    t->insert(t, "(",   NULL);
    t->insert(t, ")",   NULL);
    t->insert(t, "}",   NULL);
    t->insert(t, "[",   NULL);
    t->insert(t, "]",   NULL);

    t->insert(t, "=",   NULL);
    t->insert(t, ":=",  NULL);
    t->insert(t, "+=",  NULL);
    t->insert(t, "-=",  NULL);
    t->insert(t, "*=",  NULL);
    t->insert(t, "/=",  NULL);
    t->insert(t, "%=",  NULL);
    t->insert(t, "&=",  NULL);
    t->insert(t, "|=",  NULL);
    t->insert(t, "~=",  NULL);
    t->insert(t, "^=",  NULL);
    t->insert(t, "<<=", NULL);
    t->insert(t, ">>=", NULL);

    t->insert(t, "&&",  NULL);
    t->insert(t, "||",  NULL);

    t->insert(t, "!",   NULL);

    t->insert(t, "!=",  NULL);
    t->insert(t, "==",  NULL);
    t->insert(t, ">=",  NULL);
    t->insert(t, "<=",  NULL);
    t->insert(t, ">",   NULL);
    t->insert(t, "<",   NULL);

    t->insert(t, "+",   NULL);
    t->insert(t, "-",   NULL);
    t->insert(t, "*",   NULL);
    t->insert(t, "/",   NULL);
    t->insert(t, "%",   NULL);
    t->insert(t, "**",  NULL);

    t->insert(t, "&",   NULL);
    t->insert(t, "|",   NULL);
    t->insert(t, "^",   NULL);
    t->insert(t, "~",   NULL);

    t->insert(t, "<<",  NULL);
    t->insert(t, ">>",  NULL);

    t->insert(t, "++",  NULL);
    t->insert(t, "--",  NULL);


    t->insert(t, "//",  NULL);
    t->insert(t, ".",   NULL);
}

u8 precedence(int tt, bool isUnary, bool isPostfix) {
    switch (tt) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':

        case ',':
            return 0;

        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_BITWISE_NOT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_GREATER_THAN_OR_EQUAL:
        case '>':
        case TT_LESS_THAN_OR_EQUAL:
        case '<':
            return 3;

        case '+':
        case '-':
            if (isUnary) {
                return 7;
            }

        case '*':
        case '/':
        case '%':

        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 4;
        case '~':
            return 5;

        case TT_NOT_EQUALS:
        case TT_EQUALITY:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_AND:
            return 6;

        case '!':
            return 7;

        case TT_EXPONENTIATION:

        case TT_DECREMENT:
        case TT_INCREMENT:
        case '.':

        case TT_SYMBOL:
            // do a lookup for global symbols like 'sizeof' equivalent or stuff like that
            break;
    }

    fprintf(stderr, "attempt to lookup precedence for unknown operator/tokentype: %d\n", tt);
    exit(1);
}

