
#ifndef CLUE_H
#define CLUE_H

#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_DEBUG_MODE 0
#define CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH 120
#define CLUE_SYMBOL_TABLE_SIZE 5

#include "table.h"

Table* symbolTable;
void initSymbolTable();

#endif

