
#include <stdlib.h>

#include "node.h"
#include "token.h"

Node* parse(Token* tokens) {
    Token* currentToken = tokens;

    Node* root = {
        NULL,
        NULL,
        currentToken
    };

    return root;
}

Node* evaluate(Token* tokens) {

}

