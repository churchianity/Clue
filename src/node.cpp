
#include "clue.h"
#include "node.h"
#include "operator.h"
#include "table.h"
#include "token.h"
#include "util.h"
#include "print.h"


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (const ASTNode*)) {
    if (!self) {
        return;
    }

    if (self->children != NULL) {
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
        }
    }

    callback(self);
}

/**
 *
 */
ASTNode* newNode(Token tokens[], u32 i) {
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    node->token = &tokens[i];

    node->children = NULL;
    node->childrenCount = 0;

    if (!isOperator(&tokens[i])) { // isn't an operator... easy one
        node->maxChildrenCount = 0;

        return node;
    }

    node->token->op->isCall = false;

    // @NOTE if we pass a non-prefix operator in position 0 of the tokens array, this might be hard to catch
    if ((i < 1) || isOperator(&tokens[i - 1])) { // is unary prefix

        node->maxChildrenCount = 1;
        node->token->op->isUnary = true;
        node->token->op->isPostfix = false;

    } else {
        if (tokens[i].tt == TT_INCREMENT || tokens[i].tt == TT_DECREMENT) { // is postfix unary
            node->maxChildrenCount = 1;
            node->token->op->isUnary = true;
            node->token->op->isPostfix = true;

        } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
            node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
            node->token->op->isUnary = false;
            node->token->op->isPostfix = false;
            node->token->op->isCall = true;

        } else { // is binary
            node->maxChildrenCount = 2;
            node->token->op->isUnary = false;
            node->token->op->isPostfix = false;
        }
    }

    node->token->op->precedence = precedence(node->token->tt
                                           , node->token->op->isUnary
                                           , node->token->op->isPostfix);
    return node;
}

/**
 *
 */
void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        printf("attempting to add NULL child...\nskipping...\n");
        return;
    }

    if (self->childrenCount == self->maxChildrenCount) {
        fprintf(stderr, "attempting to add a child to a full node...\n");
        exit(1);
    }

    if (!self->children) {
        self->children = (ASTNode*) pMalloc(sizeof (ASTNode) * self->maxChildrenCount);
    }

    self->children[self->childrenCount++] = *child;
}

