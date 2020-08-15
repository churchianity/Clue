
#ifndef CLUE_H
#define CLUE_H

#define CLUE_VERSION_NUMBER "0.1.0"
#define CLUE_DEBUG_LEVEL 1
#define CLUE_FILE_SUFFIX ".clue"

#define CLUE_GLOBAL_SYMBOL_TABLE_SIZE 20
#define CLUE_INITIAL_TOKEN_ARRAY_CAPACITY 16
#define CLUE_FILE_LIST_INITIAL_CAPACITY 10
#define CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY 16
#define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 120

#define CLUE_MAX_ARGUMENT_LIST_SIZE 256


/**
 * Dependencies:
 *  C++11
 *
 *  stdlib.h
 *      malloc, calloc, realloc, free, exit, NULL
 *
 *  stdio.h
 *      stdin, stdout, stderr
 *      fgets, fprintf, printf
 *
 *  stdint.h
 *      uint64_t, uint32_t, uint16_t, uint8_t, int64_t, int32_t, int16_t, int8_t
 *
 *  stdarg.h
 *      concatenate function in (our) string.h
 *      printf wrapper
 *
 *
 *  execinfo.h
 *      backtrace, backtrace_symbols
 *
 *  signal.h
 *      SIGSEGV, SIGABRT, signal
 */
static struct {
    // whether or not to drop the user into interactive mode after reading source files (if present)
    bool interactive = false;

    // the name of the directory that is the root of the source tree of whatever project you are in
    // note that this is an assumption some percentage of the time, because the only way to be 100%
    // positive is to ask the user (and even then there's user error), which we don't want to make mandatory
    const char* src = 0;

    char** files = 0;
    int filec = 0;
} CLAs;

#endif

