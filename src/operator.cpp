
#include "operator.h"
#include "print.h"
#include "table.hpp"


/*
        PRECEDENCE       OPERATOR(S)          ASSOCIATIVITY
            .----.--------------------------.---------------.
            |    |  :                       |     none      |
            |    |--------------------------|---------------|
            |  8 |  .                       | left-to-right |
            |    |--------------------------|---------------|
(prefix)    |    |  ++ -- ( [ { @ $         |               |
            |----|--------------------------|               |
            |  7 |  **                      | right-to-left |
            |----|--------------------------|               |
(unary)     |    |  + - ~ !                 |               |
            |  6 |--------------------------|---------------|
(postfix)   |    |  ++ --                   | left-to-right |
            |----|--------------------------|---------------|
            |  5 |  * / % & ^ | << >>       |               |
            |----|--------------------------|               |
(binary)    |  4 |  + -                     |               |
            |----|--------------------------| left-to-right |
            |  3 |  != == <= < >= >         |               |
            |----|--------------------------|               |
            |  2 |  || &&                   |               |
            |----|--------------------------|---------------|
            |    |  = :=                    |               |
            |  1 |  += -= *= /= %= **=      |     none      |
            |    |  <<= >>= &= ^= |=        |               |
            |----|--------------------------|---------------|
            |  0 |  , ; ) ] }               | left-to-right |
            '----'--------------------------'---------------'

    { associativity = none } means an operator for which there should never be adjacent operators of equal precedence.
*/
Table<const char, Operator>* ot = null;


static Operator* op(u8 precedence, OperatorAssociativityEnum associativity, u32 type) {
    Operator* o = (Operator*) pMalloc(sizeof (Operator));

    o->precedence = precedence;
    o->associativity = associativity;
    o->type = type;

    return o;
}

/**
 * inits and returns a table which is information about the operators in the language,
 * without contextual modifications, where 'contextual modifications' means:
 *
 *      - it is unknown if the operator is being used in a unary or postfix context,
 *        except when the operator is only ever unary
 */
static Table<const char, Operator>* initOperatorTable() {
    auto t = new Table<const char, Operator>(25);

    t->insert("import",     6, op(9, OA_NONE, TT_IMPORT));
    t->insert("if",         2, op(9, OA_NONE, TT_IF));
    t->insert("else",       4, op(9, OA_NONE, TT_ELSE));
    t->insert("while",      5, op(9, OA_NONE, TT_WHILE));

    t->insert(",",          1, op(0, OA_NONE, ',')); // @TODO ?? associativity
    t->insert(";",          1, op(0, OA_NONE, ';'));
    t->insert(")",          1, op(0, OA_NONE, ')'));
    t->insert("]",          1, op(0, OA_NONE, ']'));
    t->insert("}",          1, op(0, OA_NONE, '}'));

    t->insert("=",          1, op(1, OA_NONE, '='));
    t->insert(":=",         2, op(1, OA_NONE, TT_COLON_EQUALS));
    t->insert("+=",         2, op(1, OA_NONE, TT_PLUS_EQUALS));
    t->insert("-=",         2, op(1, OA_NONE, TT_MINUS_EQUALS));
    t->insert("*=",         2, op(1, OA_NONE, TT_TIMES_EQUALS));
    t->insert("/=",         2, op(1, OA_NONE, TT_DIVIDE_EQUALS));
    t->insert("%=",         2, op(1, OA_NONE, TT_MODULO_EQUALS));
    t->insert("**=",        3, op(1, OA_NONE, TT_EXPONENTIATION_EQUALS));
    t->insert("&=",         2, op(1, OA_NONE, TT_BITWISE_AND_EQUALS));
    t->insert("|=",         2, op(1, OA_NONE, TT_BITWISE_OR_EQUALS));
    t->insert("^=",         2, op(1, OA_NONE, TT_BITWISE_XOR_EQUALS));
    t->insert(">>=",        3, op(1, OA_NONE, TT_RIGHT_SHIFT_EQUALS));
    t->insert("<<=",        3, op(1, OA_NONE, TT_LEFT_SHIFT_EQUALS));

    t->insert("&&",         2, op(2, OA_LEFT_TO_RIGHT, TT_LOGICAL_AND));
    t->insert("||",         2, op(2, OA_LEFT_TO_RIGHT, TT_LOGICAL_OR));

    t->insert("!=",         2, op(3, OA_LEFT_TO_RIGHT, TT_NOT_EQUALS));
    t->insert("==",         2, op(3, OA_LEFT_TO_RIGHT, TT_EQUALITY));
    t->insert("<=",         2, op(3, OA_LEFT_TO_RIGHT, TT_LESS_THAN_OR_EQUAL));
    t->insert(">=",         2, op(3, OA_LEFT_TO_RIGHT, TT_GREATER_THAN_OR_EQUAL));
    t->insert("<",          1, op(3, OA_LEFT_TO_RIGHT, '<'));
    t->insert(">",          1, op(3, OA_LEFT_TO_RIGHT, '>'));

    t->insert("+",          1, op(4, OA_LEFT_TO_RIGHT, '+'));
    t->insert("-",          1, op(4, OA_LEFT_TO_RIGHT, '-'));

    t->insert("*",          1, op(5, OA_LEFT_TO_RIGHT, '*'));
    t->insert("/",          1, op(5, OA_LEFT_TO_RIGHT, '/'));
    t->insert("%",          1, op(5, OA_LEFT_TO_RIGHT, '%'));
    t->insert("&",          1, op(5, OA_LEFT_TO_RIGHT, '&'));
    t->insert("|",          1, op(5, OA_LEFT_TO_RIGHT, '|'));
    t->insert("^",          1, op(5, OA_LEFT_TO_RIGHT, '^'));
    t->insert(">>",         2, op(5, OA_LEFT_TO_RIGHT, TT_RIGHT_SHIFT));
    t->insert("<<",         2, op(5, OA_LEFT_TO_RIGHT, TT_LEFT_SHIFT));

    t->insert("**",         2, op(7, OA_RIGHT_TO_LEFT, TT_EXPONENTIATION));

    t->insert("~",          1, op(6, OA_RIGHT_TO_LEFT, '~'));
    t->insert("!",          1, op(6, OA_RIGHT_TO_LEFT, '!'));

    t->insert("++",         2, op(8, OA_RIGHT_TO_LEFT, TT_INCREMENT)); // remember - assumed prefix
    t->insert("--",         2, op(8, OA_RIGHT_TO_LEFT, TT_DECREMENT)); // ^

    t->insert(":",          1, op(8, OA_NONE, ':'));
    t->insert("(",          1, op(8, OA_RIGHT_TO_LEFT, '(')); // function invocation
    t->insert("[",          1, op(8, OA_NONE, '['));          // indexer
    t->insert(".",          1, op(8, OA_LEFT_TO_RIGHT, '.'));
    t->insert("@",          1, op(8, OA_RIGHT_TO_LEFT, '@'));
    t->insert("$",          1, op(8, OA_RIGHT_TO_LEFT, '$'));

    return t;
}

Table<const char, Operator>* getOperatorTable() {
    if (!ot) {
        return ot = initOperatorTable();
    }

    return ot;
}

