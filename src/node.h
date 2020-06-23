
#ifndef NODE_H
#define NODE_H

#include "token.h"

typedef struct Node {
    unsigned int childrenCount;
    struct Node* children;
    Token* token;

    unsigned int bp; // binding power
    void (*nud) (); // null denotation
    void (*led) (); // left denotation

    void (*toString) (const struct Node*);
    void (*traverse) (struct Node*, void (*callback) (struct Node*));
} Node;

void freeNode(Node* node);
Node* newNode(Token* token);

#endif

