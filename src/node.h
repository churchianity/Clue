
#ifndef NODE_H
#define NODE_H

#include "token.h"

typedef struct Node {
    unsigned int childrenCount;
    struct Node* children;
    Token* token;
    void (*print) (const struct Node*);
} Node;

void traverse(Node* head, void (*callback) (Node*));
Node* newNode(Token* token);

#endif

