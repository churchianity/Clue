
#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h> // malloc, calloc, realloc, exit
#include <stdio.h> // stderr, fprintf, for now.

#include "trace.h"
#include "types.h"


inline void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        die("failed to malloc %p with size %u... zzz...\n", p, size);
    }

    return p;
}

inline void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        die("failed to calloc %p with %u elements of size %u... zzz...\n", p, maxNumOfElements, elementSize);
    }

    return p;
}

inline void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        die("failed to realloc %p with size: %u... zzz...\n", buffer, newSize);
    }

    return p;
}

#endif

