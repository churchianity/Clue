
#ifndef CLUE_H
#define CLUE_H

#define CLUE_INITIAL_TOKEN_ARRAY_SIZE 10
#define CLUE_DEBUG_MODE 0
#define CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH 120
#define CLUE_INITIAL_SYMBOL_TABLE_CAPACITY 10

#include "table.h"

Table* SymbolTable = newTable(100);

// these are the reserved tokens in the clue programming language!
// mostly operators... which can't be part of a name anyway...
SymbolTable->insert(SymbolTable, "+", "PLUS");
SymbolTable->insert(SymbolTable, "-", "MINUS");
SymbolTable->insert(SymbolTable, "*", "ASTERISK");
SymbolTable->insert(SymbolTable, "/", "FORWARD_SLASH");
SymbolTable->insert(SymbolTable, "%", "PERCENT");

#endif

