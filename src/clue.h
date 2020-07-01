
#ifndef CLUE_H
#define CLUE_H

#define CLUE_VERSION_NUMBER "0.1.0"

#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_DEBUG_LEVEL 1
#define CLUE_FILE_SUFFIX ".clue"
#define CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH 120
#define CLUE_GLOBAL_SYMBOL_TABLE_SIZE 20

#define _DIV "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * "

#include <stdbool.h>

#include "table.h"

typedef struct {
    bool interactive;
    unsigned int filenameCount;
    char* filenames;
} CommandLineArguments;

CommandLineArguments* CLAs;

Table* globalSymbolTable;

#endif

