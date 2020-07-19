
#ifndef CLUE_H
#define CLUE_H

#define CLUE_FILE_SUFFIX ".clue"
#define CLUE_VERSION_NUMBER "0.1.0"
#define CLUE_DEBUG_LEVEL 1

#define CLUE_GLOBAL_SYMBOL_TABLE_SIZE 20
#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY 16
#define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 120

#define _DIV "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * "

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/**
 * Non-struct, non-union datatypes in this codebase:
 * @NOTE We are using u32 and s32 for almost everything right now, and will optimze later.
 */
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

// char // i don't know if we should use char8_t, seems like a medium sized pain for a small sized gain
// bool
// float, double, extended
// string?

typedef struct {
    bool interactive;   // whether or not to drop the user into interactive mode after reading source files (if present)
    const char* src;    // the name of the directory that is the root of the source tree of whatever project you are in
} CommandLineArguments;

#include "table.h"
extern Table* op_t;

extern CommandLineArguments* CLAs;

#endif

