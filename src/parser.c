
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
                es->push(es, newLeafASTNode(&tokens[i], NULL)); // @TODO replace with search scope for var
                break;

            case TT_STRING:
            case TT_NUMERIC:
                es->push(es, newLeafASTNode(&tokens[i], NULL));
                break;

            case '(':
                os->push(os, &tokens[i]);
                break;

            case ')':
                while (os->peek(os)) {
                    if (((Token*) os->peek(os))->tt == '(') {
                        break;
                    }

                    /*
                    ASTNode* lhs = es->pop(es);
                    ASTNode* rhs = es->pop(es);

                    if (!lhs) {} // @TODO report parser error
                    if (!rhs) {} // @TODO report parser error

                    ASTNode* opASTNode = newASTNode(os->pop(os), NULL);

                    opASTNode->addChild(opASTNode, lhs);
                    opASTNode->addChild(opASTNode, rhs);

                    es->push(es, opASTNode);
                    */
                }

                os->pop(os); // discard opening parens
                break;

            default: // should be an operator
                if (os->isEmpty(os)) {
                    os->push(os, &tokens[i]);
                    break;
                }

                while (!os->isEmpty(os) && (precedence(os->peek(os)) > precedence(&tokens[i]))) {
                    es->push(es, os->pop(os));
                }

                os->push(os, &tokens[i]);
                break;
        }

        ++i;
    }

    // dump the operator stack into the expression stack
    while (!os->isEmpty(os)) {
        es->push(es, os->pop(os));
    }

    es->push(es, &tokens[i]); // sentinel

    // return es;
    return NULL;
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
ASTNode* parse(Token tokens[]) {
    return shuntingYard(tokens);
}

