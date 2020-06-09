
#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "token.h"

typedef struct Node {
    struct Node* left;
    struct Node* right;
    Token* token;
} Node;

void traverse(Node* head);

#endif
