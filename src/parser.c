
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "node.h"
#include "stack.h"
#include "table.h"
#include "token.h"
#include "util.h"

/**
 * Debatably this shouldn't go here.
 * @TODO there should be a lookup table for operator precedence, which is queried here and then modified by an optional
 * context (string concatenation may have different precedence than addition, but share a symbol)
 */
unsigned int operatorPrecedence(Token* token /*, Context context */) {
    if (strcmp(token->tk, "+") == 0 || strcmp(token->tk, "-") == 0) {
        return 50;

    } else if (strcmp(token->tk, "*") == 0 || strcmp(token->tk, "/") == 0 || strcmp(token->tk, "%") == 0) {
        return 60;

    } else if (strcmp(token->tk, "=") == 0) {
        return 10;
    }

    return 0;
}

/**
 *
 */
static Stack* shuntingYard(Token tokens[]) {
    Stack* es = newStack(10, true);
    Stack* os = newStack(10, true);

    unsigned int i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        switch (tokens[i].tt) {
            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(es, &tokens[i]);
                break;

            case TT_OPERATOR:
                if (tokens[i].tk[0] == '(') {
                    os->push(os, &tokens[i]);

                } else if (tokens[i].tk[0] == ')') {
                    while (((Token*) os->peek(os))->tk[0] != '(') {
                        es->push(es, os->pop(os));
                    }

                    os->pop(os); // discard opening bracket

                } else if (os->isEmpty(os)) {
                    os->push(os, &tokens[i]);

                } else {
                    while (operatorPrecedence(os->peek(os)) > operatorPrecedence(&tokens[i])) {
                        es->push(es, os->pop(os));
                    }

                    os->push(os, &tokens[i]);
                }

                break;

            case TT_SENTINEL:
            default:
                fprintf(stderr, "Unknown or illegal token-type passed into shuntingYard: %d\n", tokens[i].tt);
                exit(1);
        }

        ++i;
    }

    while (!os->isEmpty(os)) {
        es->push(es, os->pop(os));
    }

    es->push(es, newToken(-1, -1, TT_SENTINEL, "END_OF_STREAM", false));

    return es;
}

static Token* infixToPostfix(Token tokens[]) {
    Stack* es = shuntingYard(tokens);

    Token* postfixTokens = pmalloc(sizeof (Token) * es->size(es));

    for (unsigned int i = 0; i < es->size(es); i++) {
        Token* token = (Token*) *(es->data + i);

        postfixTokens[i] = *token;
    }

    return postfixTokens;
}

static Node* postfixToAST(Token tokens[]) {
    return NULL;
}

/**
 *
 */
Node* parse(Token tokens[]) {
    return postfixToAST(infixToPostfix(tokens));
}

