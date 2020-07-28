
#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h> // malloc, calloc, realloc, exit
#include <stdio.h> // stderr, fprintf (for now)
                   // can't #include "print.h" because of circular dependency...

#include "types.h"


inline void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        fprintf(stderr, "failed to malloc %p with size %u... zzz...\n", p, size);
        exit(1);
    }

    return p;
}

inline void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        fprintf(stderr, "failed to calloc %p with %u elements of size %u... zzz...\n", p, maxNumOfElements, elementSize);
        exit(1);
    }

    return p;
}

inline void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        fprintf(stderr, "failed to realloc %p with size: %u... zzz...\n", buffer, newSize);
        exit(1);
    }

    return p;
}

#endif

