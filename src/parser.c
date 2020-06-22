
#include <stdlib.h>
#include <stdio.h>

#include "clue.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "util.h"

TableEntry* symbolTable[CLUE_INITIAL_SYMBOL_TABLE_CAPACITY];
unsigned int symbolTableCapacity = CLUE_INITIAL_SYMBOL_TABLE_CAPACITY;

static void initSymbolTable() {
}

/**
 *
 */
Node* parse(Token* tokens) {
    initSymbolTable();

    for (unsigned int i = 0; i < symbolTableCapacity; ++i) {
        printf("table entry %d: %p\n", i, (void*) (symbolTable + i));
    }

    unsigned int i = 0;
    while (tokens[i].tt != TT_SENTINEL) {
        if (tokens[i].tt == TT_SYMBOL) {
            insert(symbolTable, tokens[i].tk, symbolTableCapacity);
        }

        ++i;
    }

    for (unsigned int i = 0; i < symbolTableCapacity; ++i) {
        // printf("table entry %d: %s | %p\n", i, symbolTable[i] != NULL ? symbolTable[i]->value : "none", (void*) (symbolTable + i));
    }

    return NULL;
}

