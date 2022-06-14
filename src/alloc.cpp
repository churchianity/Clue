
#include <stdlib.h> // malloc, calloc, realloc, free

#include "print.h"
#include "types.h"


void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        die("Out of memory!n\nfailed to malloc %p with size %u\n", p, size);
    }

    return p;
}

// note the argument order swap vs. calloc
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

// if you don't believe that you should be able to free const pointers then just delete this and the corresponding
// def in alloc.h
void pFreeConst(const void* ptr) {
    free((void*) ptr);
}

