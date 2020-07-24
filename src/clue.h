
#ifndef CLUE_H
#define CLUE_H

#define CLUE_FILE_SUFFIX ".clue"
#define CLUE_VERSION_NUMBER "0.1.0"
#define CLUE_DEBUG_LEVEL 1

#define CLUE_GLOBAL_SYMBOL_TABLE_SIZE 20
#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY 16
#define CLUE_MAX_ARGUMENT_LIST_SIZE 256
#define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 120

#define _DIV "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * "


/**
 * Dependencies:
 *
 * C++11
 *  seems to autoinclude from C:
 *   stdbool.h - true, false, bool
 *   stdint.h - actually comes from stdlib.h in c++11 looks like
 *    - uint64_t, uint32_t, uint16_t, uint8_t, int64_t, int32_t, int16_t, int8_t
 */
#include <stdlib.h>
/** malloc, calloc, realloc
 *  free
 *  exit
 *  NULL
 */
#include <stdio.h>
/** stdin, stdout, stderr
 *  fgets, fprintf, printf
 */
#include <stdarg.h>
/**
 *  just for a concatenate function in (our) string.h
 */
#include <execinfo.h>
/**
 *  backtrace, backtrace_symbols
 */

/**
 * Primitive (Non-struct/class, non-union) datatypes in this codebase:
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

// bool - true, false
// char // i don't know if we should use char8_t, seems like a medium sized pain for a small sized gain
// float, double, extended. probably only double.
// string? probably not.

// #include "lexer.h"
// #include "message.h"
// #include "node.h"
// #include "operator.h"
// #include "parser.h"
// #include "print.h"
// #include "reporter.h"
// #include "stack.h"
// #include "string.h"
// #include "symbol.h"
// #include "table.h"
// #include "token.h"
// #include "util.h"


/**
 * Prints a stack trace.
 */
void trace(FILE* out = stderr, u32 maxFrames = 63);

/**
 * A struct holding all of the relevant command line arguments
 */
static struct {
    // whether or not to drop the user into interactive mode after reading source files (if present)
    bool interactive;

    // the name of the directory that is the root of the source tree of whatever project you are in
    // note that this is an assumption some percentage of the time, because the only way to be 100%
    // positive is to ask the user (and even then there's user error), which we don't want to make mandatory
    const char* src;

} CLAs;

#endif

