
#ifndef CLUE_H
#define CLUE_H

#define __CLUE_VERSION_NUMBER__ "0.1.0"
#define CLUE_FILE_SUFFIX ".clue"

#define CLUE_DEBUG_LEVEL 1


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
 *      printf wrapper(s) in print.h
 *
 *  execinfo.h
 *      backtrace, backtrace_symbols
 *
 *  signal.h
 *      SIGSEGV, SIGABRT, signal()
 */
struct {
    // whether or not to drop the user into interactive mode after reading source files (if present)
    bool interactive;

    // the name of the directory that is the root of the source tree of whatever project you are in
    // note that this is an assumption some percentage of the time, because the only way to be 100%
    // positive is to ask the user (and even then there's user error), which we don't want to make mandatory
    const char* src;

    char** files;
    int filec;
} CLAs;

#endif

