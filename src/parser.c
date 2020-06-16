
#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "token.h"

Node* findRoot(Token* tokens) {
    unsigned int i = 0;

    while (tokens[i].tt != TT_NO_OP) {
        if (tokens[i].tt == TT_OPERATOR) {
            return newNode(&tokens[i]);
        }

        i++;
    }

    return NULL;
}

/**
 *
 */
Node* parse(Token* tokens) {
    Node* root = findRoot(tokens);


    return root;
}

