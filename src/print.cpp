
#include <execinfo.h> // backtrace, backtrace_symbols
#include <stdarg.h> // va_list, va_start, va_end, ...
#include <stdio.h> // FILE, stderr, stdout, stdin? | vfprintf
#include <stdlib.h> // exit
#include <stddef.h> // size_t

#include "alloc.h"
#include "print.h"
#include "token.h"
#include "types.h"


/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point with this
 */
void print(const char* format, ...) {
    if (format == null) {
        print("null\n"); return;
    }

    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);

    va_end(args);
}

/**
 * Prints a stack trace.
 * maxFrames = 63 is arbitrary
 */
void trace() {
    const u32 MAX_FRAMES = 63;
    void** stack = (void**) pMalloc(sizeof (void*) * MAX_FRAMES);
    u32 stackSize = backtrace(stack, MAX_FRAMES);

    // resolve addresses into strings containing "filename(function+address)"
    // this array must be free()-ed
    char** traces = backtrace_symbols(stack, stackSize);

    if (stackSize < 2) {
        fprintf(stderr, "stack has a weird number (%d) of frames!\n", stackSize);
        exit(1);
    }

    // iterate over the returned symbol lines. skip the first, it is the address of this function
    for (u32 i = 1; i < stackSize; i++) {
        fprintf(stderr, "  %s\n", traces[i]);
    }

    pFree(traces);
    pFree(stack);
}

/**
 * This should end the program, use for fatal **internal only** errors.
 */
void die(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);

    trace();

    exit(1);
}

void print(bool b)         { print("%s\n", boolToString(b)); }
void print(char c)         { print("%c\n", c); }
void print(signed int i)   { print("%d\n", i); }
void print(unsigned int i) { print("%u\n", i); }
void print(float f)        { print("%.14g\n", f); }
void print(double d)       { print("%.14g\n", d); }
void print(void* p)        { print("%p\n", p); }

void print(size_t i)       { print("%u\n", i); }

