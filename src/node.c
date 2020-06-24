

#include <stdio.h>
#include <stdlib.h>

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

    printf("Printing Node %p\nNode # of children: %d\n", (void*) self, self->childrenCount);
    printf("Node token: ");

    printf("%s", self->token->toString(self->token));

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
 * Creates a new node, or returns an existing one if the |token| passed in is a symbol,
 * and the symbol already exists in a passed-in |symbolTable|
 */
Node* newNode(Token* token, Table* symbolTable) {
    Node* node;

    switch (token->tt) {
        case TT_SYMBOL:
            // lookup symbol within current scope
            node = symbolTable->lookup(symbolTable, token->tk);

            if (node) {
                return node;
            }

            // construct new symbol
            node = pmalloc(sizeof (Node));
            node->childrenCount = 0;
            node->children = NULL;
            break;

        case TT_OPERATOR:
            // lookup operator.
            // users cannot define custom operators, so if we don't find something we should complain.
            node = symbolTable->lookup(symbolTable, token->tk);

            if (node) {
                return node;
            }

            fprintf(stderr, "received an unknown operator: %s\n", token->tk);
            exit(1);

        case TT_STRING:
        case TT_NUMERIC:
            node->childrenCount = 0;
            node->children = NULL;
            break;

        default:
            fprintf(stderr, "Unknown token-type passed into newNode: %d\n", token->tt);
            exit(1);
    }

    node->token = token;
    node->bp = 0;
    node->nud = NULL;
    node->led = NULL;
    node->toString = &toString;
    node->traverse = &traverse;

    return node;
}

/**
 *
 */
void freeNode(Node* node) {
    free(node);
}

