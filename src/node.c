
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
static void toString(const Node* self) {
    if (!self) {
        return;
    }

    printf("Printing Node %p\nNode # of children: %u\n", (void*) self, self->childrenCount);
    printf("Node token: ");

    printf("%s", self->token->toString(self->token));

    for (u32 i = 0; i < self->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (self->children + i));
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
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
        }
    }

    callback(self);
}

Node* newNode(Token* token, Table* symbolTable) {
    Node* node = pMalloc(sizeof (Node));

    *node = (Node) {
        .toString = &toString,
        .traverse = &traverse
    };

    return node;
}

void freeNode(Node* node) {
    free(node);
}

