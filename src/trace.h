
#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>
#include <execinfo.h> // backtrace, backtrace_symbols

#include "alloc.h"
#include "types.h"

/**
 * Prints a stack trace.
 */
inline void trace(FILE* out = stderr, u32 maxFrames = 63) {
    void** stack = (void**) pMalloc(sizeof (void*) * maxFrames);
    u32 stackSize = backtrace(stack, maxFrames / sizeof(void*));

    // resolve addresses into strings containing "filename(function+address)"
    // this array must be free()-ed
    char** traces = backtrace_symbols(stack, stackSize);

    if (stackSize < 2) {
        fprintf(stderr, "stack has a weird number (%d) of frames! and we segfaulted anyway sooo...\n", stackSize);
        exit(1);
    }

    // iterate over the returned symbol lines. skip the first, it is the address of this function
    for (u32 i = 1; i < stackSize; i++) {
        fprintf(out, "  %s\n", traces[i]);
    }

    free(traces);
    free(stack);
}

#endif

