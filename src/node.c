
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "token.h"
#include "util.h"

/**
 * Prints some relevant information about a node.
 * Doesn't recurse into children, but does show their addresses.
 */
static void print(Node* self) {
    if (!self) {
        return;
    }

    printf("Printing Node %p\nNode # of children: %d\n", (void*) self, self->childrenCount);
    printf("Node token: ");

    self->token->print(self->token);

    for (unsigned int i = 0; i < self->childrenCount; i++) {
        printf("\tChild %d pointer: %p\n", i, (void*) (self->children + i));
    }

    printf("\n");
}

/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 */
static void traverse(Node* self, void (*callback) (Node*)) {
    if (!self) {
        return;
    }

    if (self->children != NULL) {
        for (unsigned int i = 0; i < self->childrenCount; i++) {
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
Node* newNode(Token* token) {
    Node* node = pmalloc(sizeof (Node));

    switch (token->tt) {
        case TT_SENTINEL: // a sentinel shouldn't ever really be interacted with except to check it exists
            node->childrenCount = 0;
            node->children = NULL;
            node->token = token;
            node->print = &print;
            node->traverse = &traverse;
            break;

        case TT_SYMBOL:
        case TT_STRING:
        case TT_NUMERIC:
            node->childrenCount = 0;
            node->children = NULL;
            node->token = token;
            node->print = &print;
            node->traverse = &traverse;
            break;

        case TT_OPERATOR:
            node->childrenCount = 2; // @NOTE unary operators exist, but we figure out if this node is unary later
            node->children = pmalloc(sizeof (Node) * 2);
            node->token = token;
            node->print = &print;
            node->traverse = &traverse;
            break;

        default:
            fprintf(stderr, "Unknown token-type passed into newNode: %d\n", token->tt);
            exit(1);
    }

    return node;
}

/**
 *
 */
void freeNode(Node* node) {
    free(node);
}

