
#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "stack.h"
#include "token.h"
#include "util.h"

/**
 *
Node* subparse(Token* tokens, Node* root) {
    Node* currentNode = root;
    unsigned int i = 0;

    while (tokens[i].tt != NO_OP) {
        if (tokens[i - 1].tt == NUMERIC) {
            currentNode->token = &tokens[i - 1];
        }
    }

    return root;
}
*/

/**
 *
Node* parse(Token* tokens) {
    Node* root = pmalloc(sizeof (Node));
    unsigned int i = 0;

    while (tokens[i].tt != NO_OP) {
        if (tokens[i].tt == OPERATOR) {
            root->token = &tokens[i];
            break;
        }

        ++i;
    }

    return subparse(tokens+i, root);
}
 */

Node* parse(Token* tokens) {
    Stack* exprStack = newStack(10);
    Stack* operatorStack = newStack(10);

    return NULL;
}


