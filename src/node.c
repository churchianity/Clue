
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "token.h"
#include "util.h"

/**
 * Prints some relevant information about a node.
 * Doesn't recurse into children.
 */
static void print(Node* self) {
    if (!self) {
        return;
    }

    printf("Printing Node %p\nNode # of children: %d\n", self, self->childrenCount);

    for (unsigned int i = 0; i < self->childrenCount; i++) {
        printf("Child %d pointer: %p\n", i, self->children + i);
    }

    printf("Node token: ");
    self->token->print(self->token);
    printf("\n");
}

/**
 * Iterates over the tree and calls |callback| on each node, with itself as an argument.
 */
void traverse(Node* root, void (*callback) (Node*)) {
    if (!root) {
        return;
    }

    if (root->children != NULL) {
        for (unsigned int i = 0; i < root->childrenCount; i++) {
            if (root->children + i) {
                traverse(root->children + i, callback);
            }
        }
    }

    callback(root);
}

Node* newNode(Token* token) {
    Node* node = pmalloc(sizeof (Node));

    switch (token->tt) {
        case TT_SYMBOL:
        case TT_STRING:
        case TT_NUMERIC:
            node->childrenCount = 0;
            node->children = NULL;
            node->token = token;
            node->print = &print;
            return node;

        case TT_OPERATOR:
            node->childrenCount = 2;
            node->children = pmalloc(sizeof (Node) * 2);
            node->token = token;
            node->print = &print;
            return node;

        default:
            perror("uh oh big bad");
            exit(1);
    }
}

/*
 *
 */
void freeNode(Node* node) {
    free(node);
}

