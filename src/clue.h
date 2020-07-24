
#ifndef CLUE_H
#define CLUE_H

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
 *  C++11
 *
 *  <stdlib.h>
 *  malloc, calloc, realloc, free, exit, null
 *
 *  <stdio.h>
 *  stdin, stdout, stderr
 *  fgets, fprintf, printf
 *
 *  <stdint.h>
 *  uint64_t, uint32_t, uint16_t, uint8_t, int64_t, int32_t, int16_t, int8_t
 *
 *  <stdarg.h>
 *  just for a concatenate function in (our) string.h
 *
 *  <execinfo.h>
 *  backtrace, backtrace_symbols
 *
 *  <signal.h>
 *  SIGSEGV, SIGABRT
 *  signal
 */

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

