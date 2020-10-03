
#include <stdlib.h> // malloc, calloc, realloc, free

#include "print.h"
#include "types.h"

#define malloc malloc
#define calloc calloc
#define realloc realloc
#define free free

void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        die("Out of memory!n\nfailed to malloc %p with size %u\n", p, size);
    }

    return p;
}

// @NOTE argument order swap.
void* pCalloc(u32 elementSize, u32 maxNumOfElements) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        die("Out of memory!\nfailed to calloc %p with %u elements of size %u\n", p, maxNumOfElements, elementSize);
    }

    return p;
}

void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        die("Out of memory!\nfailed to realloc %p with size: %u\n", buffer, newSize);
    }

    return p;
}

void pFree(void* ptr) {
    free(ptr);
}

#undef malloc
#undef calloc
#undef realloc
#undef free

