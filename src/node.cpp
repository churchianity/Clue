
#include "array.hpp"
#include "clue.h"
#include "lexer.h"
#include "node.h"
#include "print.h"
#include "reporter.h"
#include "table.hpp"
#include "token.h"
#include "trace.h"


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    for (u32 i = 0; i < self->childrenCount; i++) {
        if (self->children + i) {
            traverse(self->children + i, callback);
        }
    }

    callback(self);
}

/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (const ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    for (u32 i = 0; i < self->childrenCount; i++) {
        if (self->children + i) {
            traverse(self->children + i, callback);
        }
    }

    callback(self);
}

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
OperatorAssociativityEnum associativity(u32 tt, bool unary, bool postfix) {
    switch (tt) {
        case ':':
        case ';':
        case TT_IMPORT:

        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_BITWISE_NOT_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return OA_NONE;

        case ',':
        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
        case '+':
        case '-':
            if (unary) {
                return OA_RIGHT_TO_LEFT;
            }

            return OA_LEFT_TO_RIGHT;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if (postfix) {
                return OA_LEFT_TO_RIGHT;
            }
        case '~':
        case '!':
        case TT_EXPONENTIATION:
        case '@':
        case '$':
        case '(':
            return OA_RIGHT_TO_LEFT;

        default:
            die("unknown operator type: %u\n", tt); return OA_NONE;
    }
}

u8 precedence(u32 tt, bool unary, bool postfix) {
    switch (tt) {
        case ',':
        case ';':
        case TT_IMPORT:
            return 0;

        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_BITWISE_NOT_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
            return 2;

        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
            return 3;

        case '+':
        case '-':
            if (unary) {
                return 6;
            }

            return 4;

        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 5;

        case '~':
        case '!':
            return 6;

        case TT_EXPONENTIATION:
            return 7;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if (postfix) {
                return 6;
            }

        case '@':
        case '$':
        case '.':
        case ':':
        case '(':
        case '[':
        case '{':
        case ')':
        case ']':
        case '}':
            return 8;

        default:
            die("unknown operator type: %u\n", tt); return 0;
    }
}

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        Reporter::add(E_MISSING_OPERAND_FOR_OPERATOR, self);
        return;
    }

    if (self->childrenCount == self->maxChildrenCount) {
        Reporter::add(E_TOO_MANY_OPERANDS, child);
    }

    if (!self->children) {
        self->children = (ASTNode*) pCalloc(sizeof (ASTNode), self->maxChildrenCount);
    }

    self->children[self->childrenCount++] = *child;
}

/**
 * Resolve a token into a node.
 * This requires some amount of peeking, so the whole Lexer::tokens array should be passed w/ the index of the operator.
 */
ASTNode* nodify(Array<Token>* tokens, u32 i) {
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    node->token = tokens->data[i];

    switch (node->token->tt) {
        default:
            break;

        case TT_SYMBOL:
        case TT_NUMERIC:
        case TT_STRING:
            return node;
    }

    if (i < 1 || isOperator(tokens->data[i - 1])) { // is unary prefix, or a punctuator used weirdly
        switch ((int) node->token->tt) {
            case '+':
            case '-':
            case '~':
            case '!':
            case TT_IMPORT:
                node->maxChildrenCount = 1;
                node->unary = true;
                break;

            case ';':
                Reporter::add(W_USELESS_SEMICOLON, node);
                break;

            default:
                Reporter::add(E_INVALID_OPERATOR, node);
                break;
        }
    } else if ((tokens->data[i]->tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) { // is a function call
        node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
        node->call = true;

    } else if ((tokens->data[i]->tt == TT_INCREMENT) || (tokens->data[i]->tt == TT_DECREMENT)) { // is postfix unary
        node->maxChildrenCount = 1;
        node->unary = true;
        node->postfix = true;

    } else { // is a binary operator or a postfix-ish punctuator, or a mistake
        node->maxChildrenCount = 2;
    }

    return node;
}

