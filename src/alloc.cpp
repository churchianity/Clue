
#include <stdlib.h> // malloc, calloc, realloc, free

#include "alloc.h"
#include "print.h"
#include "string.h"
#include "types.h"

// system allocators
void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        die("Out of memory!n\nfailed to malloc %p with size %u\n", p, size);
    }

    return p;
}

void* pMalloc(u32 size, void* allocatorState) {
    return pMalloc(size);
}

void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        die("Out of memory!\nfailed to calloc %p with %u elements of size %u\n", p, maxNumOfElements, elementSize);
    }

    return p;
}

void* pCalloc(u32 maxNumOfElements, u32 elementSize, void* allocatorState) {
    return pCalloc(maxNumOfElements, elementSize);
}

void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        die("Out of memory!\nfailed to realloc %p with size: %u\n", buffer, newSize);
    }

    return p;
}

void* pRealloc(void* buffer, u32 newSize, void* allocatorState) {
    return pRealloc(buffer, newSize);
}

void pFree(void* ptr) {
    free(ptr);
}

void pFree(void* ptr, void* allocatorState) {
    free(ptr);
}

void pFree(const void* ptr) {
    free((void*) ptr);
}

void pFree(const void* ptr, void* allocatorState) {
    free((void*) ptr);
}

// alignment should be a power of 2
static u64 alignForward(u64 ptr, u32 alignment) {
    u64 p, a, modulo;

    p = ptr;
    a = alignment;
    modulo = p & (a - 1);

    if (modulo != 0) {
        p += a - modulo;
    }

    return p;
}

// scratch allocator
Arena* arenaInit(u32 size) {
    Arena* arena = (Arena*) pMalloc(sizeof(Arena));
    arena->index = 0;
    arena->buffer = (char*) pMalloc(sizeof(char) * size);
    arena->bufferSize = size;

    return arena;
}

void* arenaAlloc(u32 size, Arena* arena) {
    char* p = arena->buffer + arena->index;
    u64 offset = alignForward((u64) p, 2 * sizeof(void*));

    if (offset + size <= arena->bufferSize) {
        void* ptr = &arena->buffer[offset];
        arena->index += offset + size;

        Str_memset(ptr, 0, size);

        return ptr;
    }

    return null;
}

