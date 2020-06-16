
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "token.h"
#include "util.h"

/**
 * Prints some relevant information about a node.
 * Doesn't recurse into children.
 */
static void print(const Node* self) {
    printf("traverse here XDXDXao\n"); fflush(stdout);
    if (!self) {
        return;
    }

    printf("Node token: %p", self->token); fflush(stdout);
    self->token->print(self->token);
    printf("Node # of children: %d\n", self->childrenCount); fflush(stdout);

    for (unsigned int i = 0; i < self->childrenCount; i++) {
        printf("Child %d pointer: %p\n", i, self->children + i); fflush(stdout);
    }
}

/**
 * @TODO fix: Right now this function isn't super safe
 */
void traverse(Node* head, void (*callback) (const Node*)) {
    if (!head) {
        return;
    }

    for (unsigned int i = 0; i < head->childrenCount; i++) {
        if (head->children + i) {
            traverse(head->children + i, callback);
        }
    }

    callback(head);
}

Node* newNode(Token* token) {
    switch (token->tt) {
        case TT_SYMBOL:
        case TT_STRING:
        case TT_NUMERIC:
            return &(Node) {
                .childrenCount = 0,
                .children = NULL,
                .token = token,
                .print = &print
            };

        case TT_OPERATOR:
            return &(Node) {
                .childrenCount = 2,
                .children = pmalloc(sizeof (Node) * 2),
                .token = token,
                .print = &print
            };

        default:
            perror("uh oh big bad");
            exit(1);
    }
}

