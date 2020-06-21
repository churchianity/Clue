
#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "stack.h"
#include "token.h"

void stackItUp(Stack* expressionStack, Stack* operatorStack, Token* tokens) {
    unsigned int i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        switch (tokens[i].tt) {
            case TT_OPERATOR:
                operatorStack->push(operatorStack, &tokens[i]);
                break;

            default:
                expressionStack->push(expressionStack, &tokens[i]);
                break;
        }

        ++i;
    }
}

/**
 *
 */
Node* parse(Token* tokens) {
    Stack* expressionStack = newStack(10);
    Stack* operatorStack = newStack(10);

    stackItUp(expressionStack, operatorStack, tokens);

    Node* root = newNode(operatorStack->pop(operatorStack));
    while (!operatorStack->isEmpty(operatorStack)) {
        Token* operator = operatorStack->pop(operatorStack);

        switch (operator->tk[0]) {
            case '+':
                while (!expressionStack->isEmpty(expressionStack)) {

                }
                break;

            case '-':
                break;

            case '*':
                break;

            case '/':
                break;

            case '%':
                break;

            default:
                break;
        }

    }

    return root;
}

