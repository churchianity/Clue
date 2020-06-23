
#include <stdlib.h>
#include <stdio.h>

#include "clue.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "util.h"

Table* symbolTable;

static void initSymbolTable(unsigned int capacity) {
    symbolTable = newTable(capacity);
}

/**
 *
 */
Node* parse(Token* tokens) {
    initSymbolTable(CLUE_INITIAL_SYMBOL_TABLE_CAPACITY);

    unsigned int i = 0;
    while (tokens[i].tt != TT_SENTINEL) {
        if (tokens[i].tt == TT_SYMBOL) {
            symbolTable->insert(symbolTable, tokens[i].tk, &tokens[i]);

            TableEntry* entry = symbolTable->lookup(symbolTable, tokens[i].tk);
            Token* token = entry == NULL ? NULL : (Token*) entry->value;

            printf("looked up key: %s and got value: %s\n", tokens[i].tk, token->toString(token));
        }

        ++i;
    }



    return NULL;
}

