
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "util.h"


static Token* tokens;
static Token* currentToken;
static unsigned int tc;

/**
  *
  */
static Token* advance(char* tk) {
    TokenTypeEnum a;
    Node* o;
    Token* t;
    char* v;

    if (tk && strcmp(tk, token->tk) != 0) {
        printf("supposedly this is bad?\n");
        exit(1);
    }

    // if the current token is the last, we add it to the symbol table and return
    // if (token->tt == TT_SENTINEL) {
    //    symbolTable->insert(symbolTable, token->tk, newNode(token));
    //    return NULL;
    // }

    t = tokens[tc];

    switch (t.tt) {
        case TT_SYMBOL:
            // we check if the symbol in question is already visible within the scope
            // by querying a table for the token's 'tk' (the string that represents itself)
            break;

        case TT_OPERATOR:
            // same as above, but operators can't really be user-defined - so if we don't find
            // one in a table, we should complain
            break;

        case TT_STRING:
        case TT_NUMERIC:
            break;
    }

    return NULL;
}


/**
 *
 */
Node* parse(Token* tokens) {
    symbolTable = pmalloc(sizeof (TableEntry) * CLUE_INITIAL_SYMBOL_TABLE_CAPACITY);
    tokens = tokens;

    while (tokens[i].tt != TT_SENTINEL) {
        advance(tokens[i].tk);

        ++i;
    }

    return NULL;
}

