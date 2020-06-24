
#include <string.h>
#include <stdio.h>

#include "clue.h"
#include "runtime.h"
#include "table.h"

Table* symbolTable = NULL;

void initSymbolTable() {
    symbolTable = newTable(CLUE_INITIAL_SYMBOL_TABLE_CAPACITY);

    // these are the reserved tokens in the clue programming language!
    // mostly operators... which can't be part of a name anyway...
    symbolTable->insert(symbolTable, "+", "ADD");
    symbolTable->insert(symbolTable, "-", "MINUS");
    symbolTable->insert(symbolTable, "*", "ASTERISK");
    symbolTable->insert(symbolTable, "/", "FORWARD_SLASH");
    symbolTable->insert(symbolTable, "%", "PERCENT");
}

/**
 *
 */
int main(int argc, char* argv[]) {
    interactive(); return 0;
}

