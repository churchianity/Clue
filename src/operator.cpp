
#include "operator.h"
#include "table.hpp"


Table<const char, Operator>* OperatorTable = new Table<const char, Operator>(10);


static Operator* op(u8 precedence, OperatorAssociativityEnum associativity = OA_LEFT_TO_RIGHT) {
    Operator* o = (Operator*) pMalloc(sizeof (Operator));

    o->precedence = precedence;
    o->associativity = associativity;

    return o;
}

/*
        PRECEDENCE       OPERATOR(S)          ASSOCIATIVITY
            .----.--------------------------.---------------.
(prefix)    |  8 |  ++ -- : ( [ { . @ $     |               |
            |----|--------------------------|---------------|
            |  7 |  **                      | right-to-left |
            |----|--------------------------|---------------|
(postfix)   |  6 |  ++ -- + - ~ !           | n/a (or r->l  |
            |----|--------------------------|---------------|
            |  5 |  * / % & ^ | << >>       |               |
            |----|--------------------------|               |
            |  4 |  + -                     |               |
            |----|--------------------------| left-to-right |
            |  3 |  != == <= < >= >         |               |
            |----|--------------------------|               |
            |  2 |  || &&                   |               |
            |----|--------------------------|---------------|
            |    |  = :=                    |               |
            |  1 |  += -= *= /= %= **=      | right-to-left |
            |    |  <<= >>= &= ^= |=        |               |
            |----|--------------------------|---------------|
            |  0 |  , ; ) ] }               | left-to-right |
            '----'--------------------------'---------------'
*/

/**
 * inits and returns a table which is information about the operators in the language,
 * without contextual modifications, where 'contextual modifications' means:
 *      - it is unknown if the operator is being used in unary or postfix mode.
 *        if it only functions as a unary operator, then it is assumed to be unary prefix.
 */
void initOperatorTable(Table<const char, Operator>* t = OperatorTable) {
    t->insert("import",     6, op(9));
    t->insert("if",         2, op(9));
    t->insert("else",       4, op(9));
    t->insert("while",      5, op(9));

    t->insert(",",          1, op(0)); // @TODO ?? associativity
    t->insert(";",          1, op(0));
    t->insert(")",          1, op(0));
    t->insert("]",          1, op(0));
    t->insert("}",          1, op(0));

    t->insert("=",          1, op(1, OA_NONE));
    t->insert(":=",         2, op(1, OA_NONE));
    t->insert("+=",         2, op(1, OA_NONE));
    t->insert("-=",         2, op(1, OA_NONE));
    t->insert("*=",         2, op(1, OA_NONE));
    t->insert("/=",         2, op(1, OA_NONE));
    t->insert("%=",         2, op(1, OA_NONE));
    t->insert("**=",        3, op(1, OA_NONE));
    t->insert("&=",         2, op(1, OA_NONE));
    t->insert("|=",         2, op(1, OA_NONE));
    t->insert("^=",         2, op(1, OA_NONE));
    t->insert(">>=",        3, op(1, OA_NONE));
    t->insert("<<=",        3, op(1, OA_NONE));

    t->insert("&&",         2, op(2, OA_LEFT_TO_RIGHT));
    t->insert("||",         2, op(2, OA_LEFT_TO_RIGHT));

    t->insert("!=",         2, op(3, OA_LEFT_TO_RIGHT));
    t->insert("==",         2, op(3, OA_LEFT_TO_RIGHT));
    t->insert("<=",         2, op(3, OA_LEFT_TO_RIGHT));
    t->insert(">=",         2, op(3, OA_LEFT_TO_RIGHT));
    t->insert("<",          1, op(3, OA_LEFT_TO_RIGHT));
    t->insert(">",          1, op(3, OA_LEFT_TO_RIGHT));

    t->insert("+",          1, op(4, OA_LEFT_TO_RIGHT));
    t->insert("-",          1, op(4, OA_LEFT_TO_RIGHT));

    t->insert("*",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert("/",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert("%",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert("&",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert("|",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert("^",          1, op(5, OA_LEFT_TO_RIGHT));
    t->insert(">>",         2, op(5, OA_LEFT_TO_RIGHT));
    t->insert("<<",         2, op(5, OA_LEFT_TO_RIGHT));

    t->insert("**",         2, op(7, OA_RIGHT_TO_LEFT));

    t->insert("~",          1, op(6, OA_RIGHT_TO_LEFT));
    t->insert("!",          1, op(6, OA_RIGHT_TO_LEFT));

    t->insert("++",         2, op(8, OA_RIGHT_TO_LEFT)); // remember - assumed prefix
    t->insert("--",         2, op(8, OA_RIGHT_TO_LEFT)); // ^

    t->insert(":",          1, op(8, OA_NONE));
    t->insert("(",          1, op(8, OA_RIGHT_TO_LEFT)); // function invocation
    t->insert("[",          1, op(8, OA_NONE));          // indexer
    t->insert(".",          1, op(8, OA_LEFT_TO_RIGHT));
    t->insert("@",          1, op(8, OA_RIGHT_TO_LEFT));
    t->insert("$",          1, op(8, OA_RIGHT_TO_LEFT));
}

