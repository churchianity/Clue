
#include "clue.h"
#include "node.h"
#include "operator.h"
#include "table.h"
#include "token.h"
#include "trace.h"
#include "util.h"
#include "print.h"


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
            // do a lookup for global symbols like 'sizeof' equivalent or stuff like that
            printf("symbol operator!\n");
            break;
    }

    fprintf(stderr, "attempt to lookup precedence for unknown operator/tokentype: %c/%d\n", tt, tt);
    exit(1);
}

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        printf("attempting to add null child...\n");
        exit(1);
    }

    if (self->childrenCount == self->maxChildrenCount) {
        trace();
        fprintf(stderr, "attempting to add a child to a full node...\n");
        print(self); print(child);
        exit(1);
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
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    node->token = &tokens[i];

    node->children = null;
    node->childrenCount = 0;

    if (!isOperator(&tokens[i])) { // isn't an operator... easy one
        node->maxChildrenCount = 0;

        return node;
    }

    node->token->op->call = false;

    // @NOTE if we pass a non-prefix operator in position 0 of the tokens array, this might be hard to catch
    if ((i < 1) || isOperator(&tokens[i - 1])) { // is unary prefix
        node->maxChildrenCount = 1;
        node->token->op->unary = true;
        node->token->op->postfix = false;

    } else {
        if (tokens[i].tt == TT_INCREMENT || tokens[i].tt == TT_DECREMENT) { // is postfix unary
            node->maxChildrenCount = 1;
            node->token->op->unary = true;
            node->token->op->postfix = true;

        } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
            node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
            node->token->op->unary = false;
            node->token->op->postfix = false;
            node->token->op->call = true;

        } else { // is binary
            node->maxChildrenCount = 2;
            node->token->op->unary = false;
            node->token->op->postfix = false;
        }
    }

    node->token->op->precedence = precedence(node->token->tt
                                           , node->token->op->unary
                                           , node->token->op->postfix);
    return node;
}

