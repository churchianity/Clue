
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

/**
 *
 */
u32 precedence(Token* token) {
    Table* t = CLUE_GLOBAL_SYMBOL_TABLE_IDENTIFIER;

    TableEntry* entry = t->lookup(t, token->tk);

    if (!entry) {
        fprintf(stderr, "attempted to look up precedence for token and found nothing: %s\n", token->toString(token));
    }

    return ((Symbol*) (entry->value))->precedence;
}

/**
 * Parses expressions into an AST.
 */
static ASTNode* shuntingYard(Token tokens[]) {
    Stack* es = newStack(10, true);
    Stack* os = newStack(10, true);

    u32 i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        switch ((int) tokens[i].tt) {
            case TT_SENTINEL:
                fprintf(stderr, "Got a sentinel token inside of shuntingYard\nExiting...");
                exit(1);

            case TT_SYMBOL:
                es->push(es, newNode(&tokens[i], NULL)); // @TODO replace with search scope for var
                break;

            case TT_STRING:
            case TT_NUMERIC:
                es->push(es, newNode(&tokens[i], NULL));
                break;

            case '(':
                os->push(os, &tokens[i]);
                break;

            case ')':
                while (os->peek(os)) {
                    if (((Token*) os->peek(os))->tt == '(') {
                        break;
                    }

                    ASTNode* rhs = es->pop(es);
                    ASTNode* lhs = es->pop(es);

                    if (!rhs) { fprintf(stderr, "failed to get rhs for operator"); exit(1); } // @TODO report parser error
                    if (!lhs) { fprintf(stderr, "failed to get lhs for operator"); exit(1); } // @TODO report parser error

                    ASTNode* opNode = newNode(os->pop(os), NULL);

                    opNode->addChild(opNode, lhs);
                    opNode->addChild(opNode, rhs);

                    es->push(es, opNode);
                }

                os->pop(os); // discard opening parens
                break;

            default: // should be an operator
                if (os->isEmpty(os)) {
                    os->push(os, &tokens[i]);
                    break;
                }

                while (!os->isEmpty(os) && (precedence(os->peek(os)) > precedence(&tokens[i]))) {
                    ASTNode* rhs = es->pop(es);
                    ASTNode* lhs = es->pop(es);

                    if (!rhs) { fprintf(stderr, "failed to get rhs for operator"); exit(1); } // @TODO report parser error
                    if (!lhs) { fprintf(stderr, "failed to get lhs for operator"); exit(1); } // @TODO report parser error

                    ASTNode* opNode = newNode(os->pop(os), NULL);

                    opNode->addChild(opNode, lhs);
                    opNode->addChild(opNode, rhs);

                    es->push(es, opNode);
                }

                os->push(os, &tokens[i]);
                break;
        }

        ++i;
    }

    // it's possible there's an operator leftover we haven't dealt with yet
     while (!os->isEmpty(os)) {
        ASTNode* rhs = es->pop(es);
        ASTNode* lhs = es->pop(es);

        if (!rhs) { fprintf(stderr, "failed to get rhs for operator"); exit(1); } // @TODO report parser error
        if (!lhs) { fprintf(stderr, "failed to get lhs for operator"); exit(1); } // @TODO report parser error

        ASTNode* opNode = newNode(os->pop(os), NULL);

        opNode->addChild(opNode, lhs);
        opNode->addChild(opNode, rhs);

        es->push(es, opNode);
    }

    return (ASTNode*) es->pop(es);
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
ASTNode* parse(Token tokens[]) {
    return shuntingYard(tokens);
}

