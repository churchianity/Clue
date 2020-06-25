
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
 * context (string concatenation may have different precedence than addition, but share an operator)
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
static Token* shuntingYard(Token* tokens) {
    Token* outputBuffer = malloc(sizeof (Token) * CLUE_INITIAL_TOKEN_ARRAY_CAPACITY);
    Stack* stack = newStack(10, true);

    unsigned int i = 0;
    unsigned int tc = 0;
    unsigned int capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;

    while (tokens[i].tt != TT_SENTINEL) {
        switch (tokens[i].tt) {
            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                if (capacity < tc) {
                    capacity *= 2; // @TODO I don't know what the optimal growth rate is
                    outputBuffer = realloc(tokens, sizeof (Token) * capacity);

                    if (!outputBuffer) {
                        fprintf(stderr, "failed to realloc tokens. exiting...\n");
                        exit(1);
                    }
                }

                outputBuffer[tc++] = tokens[i];
                break;

            case TT_OPERATOR:
                if (tokens[i].tk[0] == '(') {
                    stack->push(stack, &tokens[i]);

                } else if (tokens[i].tk[0] == ')') {
                    while (((Token*) stack->peek(stack))->tk[0] != '(') {
                        if (capacity < tc) {
                            capacity *= 2; // @TODO I don't know what the optimal growth rate is
                            outputBuffer = realloc(tokens, sizeof (Token) * capacity);

                            if (!outputBuffer) {
                                fprintf(stderr, "failed to realloc tokens. exiting...\n");
                                exit(1);
                            }
                        }

                        outputBuffer[tc++] = *(Token*) stack->pop(stack);
                    }

                    stack->pop(stack); // discard opening bracket

                } else if (stack->isEmpty(stack)) {
                    stack->push(stack, &tokens[i]);

                } else {
                    while (operatorPrecedence(stack->peek(stack)) > operatorPrecedence(&tokens[i])) {
                        if (capacity < tc) {
                            capacity *= 2; // @TODO I don't know what the optimal growth rate is
                            outputBuffer = realloc(tokens, sizeof (Token) * capacity);

                            if (!outputBuffer) {
                                fprintf(stderr, "failed to realloc tokens. exiting...\n");
                                exit(1);
                            }
                        }

                        outputBuffer[tc++] = *(Token*) stack->pop(stack);
                    }

                    stack->push(stack, &tokens[i]);
                }

                break;

            case TT_SENTINEL:
            default:
                fprintf(stderr, "Unknown or illegal token-type passed into shuntingYard: %d\n", tokens[i].tt);
                exit(1);
        }

        ++i;
    }

    while (!stack->isEmpty(stack)) {
        if (capacity < tc) {
            capacity *= 2; // @TODO I don't know what the optimal growth rate is
            outputBuffer = realloc(outputBuffer, sizeof (Token) * capacity);

            if (!outputBuffer) {
                fprintf(stderr, "failed to realloc tokens. exiting...\n");
                exit(1);
            }
        }
        outputBuffer[tc++] = *(Token*) stack->pop(stack);
    }

    outputBuffer[tc] = *newToken(-1, -1, TT_SENTINEL, "END_OF_STREAM", false);

    return outputBuffer;
}

/**
 *
 */
Node* parse(Token* tokens) {
    Token* postfixTokens = shuntingYard(tokens);

    printf("\nprinting postfix tokens...\n\n");
    unsigned int i = 0;
    while (postfixTokens[i].tt != TT_SENTINEL) {
        // do something with the token probably
        printf("%s", postfixTokens[i].toString(&postfixTokens[i]));
        ++i;
    }


    // set up what needs to be set up

    i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        // do something with the token probably

        ++i;
    }

    return NULL; // the root of an AST probably
}

