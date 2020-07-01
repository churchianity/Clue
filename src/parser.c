
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "stack.h"
#include "symbol.h"
#include "table.h"
#include "token.h"
#include "util.h"

unsigned int precedence(Token* token) {
    TableEntry* entry = globalSymbolTable->lookup(globalSymbolTable, token->tk);

    if (!entry) {
        fprintf(stderr, "attempted to look up precedence for token and found nothing: %s\n", token->toString(token));
    }

    return ((Symbol*) (entry->value))->precedence;
}

static Stack* shuntingYard(Token tokens[]) {
    Stack* es = newStack(10, true);
    Stack* os = newStack(10, true);

    unsigned int i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        switch (tokens[i].tt) {
            case TT_STRING:
            case TT_SYMBOL:
            case TT_NUMERIC:
                es->push(es, &tokens[i]);
                break;

            case TT_OPERATOR:
                if (tokens[i].tk[0] == '(') {
                    os->push(os, &tokens[i]);
                    break;

                } else if (tokens[i].tk[0] == ')') {
                    while (os->peek(os)) {
                        if (((Token*) os->peek(os))->tk[0] == '(') {
                            break;
                        }

                        es->push(es, os->pop(os));
                    }

                    os->pop(os); // discard opening parens
                    break;
                }

                if (os->isEmpty(os)) {
                    os->push(os, &tokens[i]);
                    break;
                }

                while (!os->isEmpty(os) && (precedence(os->peek(os)) > precedence(&tokens[i]))) {
                    es->push(es, os->pop(os));
                }

                os->push(os, &tokens[i]);
                break;

            case TT_SENTINEL:
            default:
                fprintf(stderr, "Unknown or illegal token-type passed into shuntingYard: %d\n", tokens[i].tt);
                exit(1);
        }

        ++i;
    }

    // dump the operator stack into the expression stack
    while (!os->isEmpty(os)) {
        es->push(es, os->pop(os));
    }

    es->push(es, &tokens[i]); // sentinel

    return es;
}

static Token* infixToPostfix(Token tokens[]) {
    Stack* es = shuntingYard(tokens);

    Token* postfixTokens = pMalloc(sizeof (Token) * es->size(es));

    for (unsigned int i = 0; i < es->size(es); i++) {
        Token* token = (Token*) *(es->data + i);

        postfixTokens[i] = *token;
    }

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tPrinting postfix tokens @infixToPostfix...\n%s\n", _DIV);
        printTokens(postfixTokens);
    #endif

    return postfixTokens;
}

static Node* postfixToAST(Token tokens[]) {
    return NULL;
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
Node* parse(Token tokens[]) {
    return postfixToAST(infixToPostfix(tokens));
}

