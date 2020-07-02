
#ifndef CLUE_H
#define CLUE_H

#define CLUE_FILE_SUFFIX ".clue"
#define CLUE_VERSION_NUMBER "0.1.0"
#define CLUE_DEBUG_LEVEL 1

#define CLUE_GLOBAL_SYMBOL_TABLE_SIZE 20
#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 120

#define _DIV "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * "

#include <stdbool.h>
#include <stdint.h>


typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef struct {
    bool interactive;
    u64 filenameCount;
    char* filenames;
} CommandLineArguments;

CommandLineArguments* CLAs;

#include "table.h"
Table* globalSymbolTable;

#endif

