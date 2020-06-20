
#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "token.h"

Node* findRoot(Token* tokens) {
    unsigned int i = 0;
    Node* node;

    while (tokens[i].tt != TT_SENTINEL) {

        if (tokens[i].tt == TT_OPERATOR) {
            node = newNode(&tokens[i]);

            // we're going to assume that there should be an expression to the right
            // and maybe one to the left too
            if (i > 0 && tokens[i - 1].tt != TT_OPERATOR) {
                node->children[0] = *newNode(&tokens[i - 1]);
            }

            if (tokens[i + 1].tt != TT_SENTINEL && tokens[i + 1].tt != TT_OPERATOR) {
                node->children[1] = *newNode(&tokens[i + 1]);
            }
        }

        i++;
    }

    return node;
}

/**
 *
 */
Node* parse(Token* tokens) {
    Node* root = findRoot(tokens);

    return root;
}

