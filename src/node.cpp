
#include "clue.h"
#include "node.h"
#include "operator.h"
#include "print.h"
#include "table.hpp"
#include "token.h"
#include "trace.h"
#include "util.h"

void traverse(ASTNode* self, void (*callback) (const ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    callback(self);
}


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTOperatorNode* self, void (*callback) (const ASTNode*)) {
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
u8 precedence(u32 tt, bool unary, bool postfix) {
    switch (tt) {
        case ')':
        case ']':
        case '{':
        case '}': //????
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
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_BITWISE_NOT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_GREATER_THAN_OR_EQUAL:
        case '>':
        case TT_LESS_THAN_OR_EQUAL:
        case '<':
            return 2;

        case '+':
        case '-':
            if (unary) {
                return 7;
            }

            return 3;

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
            return 8;

        case '(':
        case '[':
        case TT_DECREMENT:
        case TT_INCREMENT:
        case '.':
        case ':':
            return 9;

        case TT_SYMBOL:
            // @FIXME do a lookup for global symbols like 'sizeof' equivalent or stuff like that
            die("symbol operator!\n");
            break;
    }

    die("attempt to lookup precedence for unknown operator/tokentype: %c/%d\n", tt, tt);
    return 0; // @NOTE will never get here, die() should always call exit(1) but gcc can't figure that out
}

void addChild(ASTOperatorNode* self, ASTNode* child) {
    if (!child) {
        return;
    }

    if (self->childrenCount == self->maxChildrenCount) {
        die("attempting to add a child to a full node...\n");
    }

    if (!self->children) {
        self->children = (ASTNode*) pMalloc(sizeof (ASTNode) * self->maxChildrenCount);
    }

    self->children[self->childrenCount++] = *child;
}

/**
 * Resolve a token into a node.
 * Most of the work here is resolving operator precedence, associativity, and unary/postfix flags.
 */
ASTNode* nodify(Token tokens[], u32 i) {
    ASTNode* node = null;           // @FIXME
    ASTSymbolNode* snode = null;    // @FIXME shouldn't have to define these here

    switch (tokens[i].tt) {
        case TT_SYMBOL:
            snode = (ASTSymbolNode*) pMalloc(sizeof (ASTSymbolNode));
            snode->token = &tokens[i];
            // @TODO do some symbol shit
            return snode;

        case TT_NUMERIC:
        case TT_STRING:
            node = (ASTNode*) pMalloc(sizeof (ASTNode));
            node->token = &tokens[i];
            return node;

        default:
            break;
    }

    // operators...
    ASTOperatorNode* onode = (ASTOperatorNode*) pMalloc(sizeof (ASTOperatorNode));

    onode->children = null;
    onode->childrenCount = 0;
    onode->op->call = false;

    // @NOTE if we pass a non-prefix operator in position 0 of the tokens array, this might be hard to catch
    if ((i < 1) || isOperator(&tokens[i - 1])) { // is unary prefix
        onode->maxChildrenCount = 1;
        onode->op->unary = true;
        onode->op->postfix = false;

    } else {
        if (tokens[i].tt == TT_INCREMENT || tokens[i].tt == TT_DECREMENT) { // is postfix unary
            onode->maxChildrenCount = 1;
            onode->op->unary = true;
            onode->op->postfix = true;

        } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
            onode->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
            onode->op->unary = false;
            onode->op->postfix = false;
            onode->op->call = true;

        } else { // is binary
            onode->maxChildrenCount = 2;
            onode->op->unary = false;
            onode->op->postfix = false;
        }
    }

    onode->op->precedence = precedence(onode->token->tt
                                     , onode->op->unary
                                     , onode->op->postfix);

    // @TODO calculate associativity here too

    return onode;
}

