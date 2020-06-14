
#include <stdio.h>

#include "node.h"
#include "token.h"

void traverse(Node* head) {
    if (head->left) {
        traverse(head->left);

    } else if (head->right) {
        traverse(head->right);

    } else if (head->token) {
        head->token->print(head->token);

    } else {
        printf("AST is empty as fuck lmao\n");
    }
}

