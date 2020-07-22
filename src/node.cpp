
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "util.h"
#include "print.h"


/**
 *
 */
u32 precedence(Token* token) {
    Table* t = op_t;

    TableEntry* entry = t->lookup(t, token->tk);

    if (!entry) {
        fprintf(stderr, "attempted to look up precedence for token and found nothing:\n");
        print(token);
        exit(1);
    }

    return ((Symbol*) (entry->value))->precedence;
}

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
    node->call = false;

    if (!isOperator(&tokens[i])) { // isn't an operator... easy one
        node->maxChildrenCount = 0;
        node->precedence = 0;
        return node;
    }

    node->precedence = precedence(&tokens[i]);

    if ((i < 1) || isOperator(&tokens[i - 1])) { // is unary prefix
        node->maxChildrenCount = 1;
        node->unary = true;
        node->postfix = false;

    } else {
        if (tokens[i].tt == TT_INCREMENT || tokens[i].tt == TT_DECREMENT) { // is postfix unary
            node->maxChildrenCount = 1;
            node->unary = true;
            node->postfix = true;

        } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
            node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
            node->unary = false;
            node->postfix = false;
            node->call = true;

        } else { // is binary
            node->maxChildrenCount = 2;
            node->unary = false;
            node->postfix = false;
        }
    }

    return node;
}

/**
 *
 */
void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        printf("attempting to add null child...\nskipping...\n");
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

