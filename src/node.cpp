
#include "clue.h"
#include "lexer.h"
#include "node.h"
#include "print.h"
#include "reporter.h"
#include "table.hpp"
#include "token.h"
#include "trace.h"
#include "util.h"


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    if (self->children != null) {
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
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

    if (self->children != null) {
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
        }
    }

    callback(self);
}

/**
 * There are three pieces of information needed to determine an operator's precedence:
 *     1. Its base type, which overlaps with TokenTypeEnum
 *     2. Whether or not it is being used as a unary operator
 *     3. Whether or not it is being used in postfix
 */

/*
PRECEDENCE       OPERATOR(S)          ASSOCIATIVITY
    +----+--------------------------+---------------+
    |  8 |  ++ -- : ( [ { . @ $     |               |
    |----|--------------------------|---------------+
    |  7 |  **                      | right-to-left |
    |----|--------------------------|---------------+
    |  6 |  ++ -- + - ~ !           | n/a (or r->l  |
    |----|--------------------------|---------------+
    |  5 |  * / % & ^ | << >>       |               |
    |----|--------------------------|               |
    |  4 |  + -                     |               |
    |----|--------------------------| left-to-right |
    |  3 |  != == <= < >= >         |               |
    |----|--------------------------|               |
    |  2 |  || &&                   |               |
    |----|--------------------------|---------------+
    |    |  = :=                    |               |
    |  1 |  += -= *= /= %= **=      | right-to-left |
    |    |  <<= >>= &= ^= |=        |               |
    |----|--------------------------|---------------|
    |  0 |  , ; ) ] }               | left-to-right |
    +----+--------------------------+---------------+
 */
u8 precedence(u32 tt, bool unary, bool postfix) {
    switch (tt) {
        case ';':
        case ',':
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

        // logical operator (non-comparison)
        case TT_LOGICAL_OR:
        case TT_LOGICAL_AND:
            return 2;

        // equality & comparison
        // these can be the same because you basically never use them in tandem...
        case TT_NOT_EQUALS:
        case TT_EQUALITY:
        case TT_GREATER_THAN_OR_EQUAL:
        case '>':
        case TT_LESS_THAN_OR_EQUAL:
        case '<':
            return 3;

        // unary & binary plus & minus
        case '+':
        case '-':
            if (unary) {
                return 6;
            }

            // binary...
            return 4;

        // other arithmetic... BEGIN LEFT_TO_RIGHT
        case '*':
        case '/':
        case '%':

        // bitwise binary operations...
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 5;

        // END LEFT_TO_RIGHT

        // unary bitwise/negation
        case '!':
        case '~':
            return 6;

        // exponentation
        case TT_EXPONENTIATION:
            return 7;

        // i'm not entirely certain why, but you get problems if ( is lower than assignment
        // when it's invoking a function it's pretty high precedence tho
        case TT_DECREMENT:
        case TT_INCREMENT:
            if (postfix) {
                return 8;
            }

        case '(':
        case '[':

        case '.':
        case ':': // @NOTE should it be this high tho?
            return 9;
    }

    die("attempt to lookup precedence for unknown operator/tokentype: %c/%d\n", tt, tt);
    return 0; // @NOTE will never get here, die() should always call exit(1) but gcc can't figure that out
}

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        Reporter::add(
            MS_ERROR, "missing operand for operator",
            null, self->token->filename, self->token->line, self->token->column
        );

        return;
    }

    if (self->childrenCount == self->maxChildrenCount) {
        Reporter::report(
            MS_ERROR, "attempting to add an operand to an operator that is already satisfied",
            null, self->token->filename, self->token->line, self->token->column
        );
    }

    if (!self->children) {
        self->children = (ASTNode*) pCalloc(self->maxChildrenCount, sizeof (ASTNode));
    }

    self->children[self->childrenCount++] = *child;
}

/**
 * Resolve a token into a node.
 *
 * Nodes are mostly distinguished by whether or not they have children.
 *
 * Most of the work here is resolving operator precedence, associativity, and unary/postfix flags.
 * That requires some amount of peeking, so the whole Lexer::tokens array should be passed w/ the index of the operator.
 */
ASTNode* nodify(Token tokens[], u32 i) {
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    node->token = &tokens[i];

    switch (node->token->tt) {
        default:
            break;

        case TT_SYMBOL:
        case TT_NUMERIC:
        case TT_STRING:
            return node;
    }

    if (i < 1 || isOperator(&tokens[i - 1])) { // is unary prefix, or a punctuator used weirdly
        switch ((int) node->token->tt) {
            case '+':
            case '-':
            case '~':
            case '!':
                node->maxChildrenCount = 1;
                node->unary = true;
                break;

            case ';':
                Reporter::add(
                    MS_WARN, "semicolon with nothing before it has no effect",
                    null, node->token->filename, node->token->line, node->token->column
                );

            case '{':
            case '(':
                node->punctuator = true;
                break;

            default:
                Reporter::report(
                    MS_ERROR, "invalid operator",
                    null, node->token->filename, node->token->line, node->token->column
                );

                break;
        }
    } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
        node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
        node->call = true;

    } else if ((tokens[i].tt == TT_INCREMENT) || (tokens[i].tt == TT_DECREMENT)) { // is postfix unary
        node->maxChildrenCount = 1;
        node->unary = true;
        node->postfix = true;

    } else { // is a binary operator or a postfix-ish punctuator
        switch ((int) tokens[i].tt) {
            case ';':
                node->punctuator = true;
                break;

            default:
                node->maxChildrenCount = 2;
                break;
        }
    }

    node->precedence = precedence(node->token->tt
                                , node->unary
                                , node->postfix);

    // @TODO calculate associativity here too

    return node;
}

void freeNode(ASTNode* node) {
    //
}

