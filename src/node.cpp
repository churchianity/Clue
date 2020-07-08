
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "util.h"

/**
 * @TODO this actually just prints stuff but instead it should:
 *
 * Returns some relevant information about a node as a single string.
 * Doesn't recurse into children, but does show their addresses.
 */
static void toString(ASTNode* self) {
    if (!self) {
        return;
    }

    printf("Printing ASTNode %p\nASTNode # of children: %u\n", (void*) self, self->childrenCount);
    printf("ASTNode token: ");

    printf("%s", self->token->toString(self->token));

    for (u32 i = 0; i < self->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (self->children + i));
    }

    printf("\n");
}

/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
static void traverse(ASTNode* self, void (*callback) (ASTNode*)) {
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

static bool addChild(struct ASTNode* self, struct ASTNode* child) {
    if (self->childrenCount == self->maxChildrenCount) {
        return false;
    }

    if (!self->children) {
        self->children = (ASTNode*) pMalloc(sizeof (ASTNode) * self->maxChildrenCount);
    }

    self->children[self->childrenCount++] = *child;
    return true;
}

ASTNode* newNode(Token* token, Table* symbolTable) {
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    if (symbolTable) {}

    node->token = token;

    node->children = NULL;
    node->childrenCount = 0;

    switch (token->tt) {
        case TT_SENTINEL:
        case TT_SYMBOL:
        case TT_NUMERIC:
        case TT_STRING:
            node->maxChildrenCount = 0;
            break;

        default:
            node->maxChildrenCount = 2;

    }

    node->toString = &toString;
    node->traverse = &traverse;
    node->addChild = &addChild;

    return node;
}

