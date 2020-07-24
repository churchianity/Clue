
#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h> // malloc, calloc, realloc, exit

#include "types.h"


inline void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        exit(1);
    }

    return p;
}

inline void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        exit(1);
    }

    return p;
}

inline void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        exit(1);
    }

    return p;
}

#endif

