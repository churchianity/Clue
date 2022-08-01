
#ifndef ALLOC_H
#define ALLOC_H

#include "types.h"


// define a consistent memory allocation interface
// trailing void* is a pointer to some allocator state, if relevant.·
// will be unused for malloc/calloc/realloc/free, as the allocator state is internal to the OS
// overloads should exist which do nothing with the trailing paramter, in that case.
typedef void* (*mallocator)(u32, void*);
typedef void* (*callocator)(u32, u32, void*);
typedef void* (*reallocator)(void*, u32, void*);
typedef void (*freeer)(void*, void*);

// system allocator wrappers
void* pMalloc(u32 size);
void* pMalloc(u32 size, void* allocatorState);
void* pCalloc(u32 maxNumOfElements, u32 elementSize);
void* pCalloc(u32 maxNumOfElements, u32 elementSize, void* allocatorState);
void* pRealloc(void* buffer, u32 newSize);
void* pRealloc(void* buffer, u32 newSize, void* allocatorState);
void pFree(void* ptr);
void pFree(void* ptr, void* allocatorState);
void pFree(const void* ptr);
void pFree(const void* ptr, void* allocatorState);

// Scratch/Arena
typedef struct Arena {
    char* buffer;
    u32 bufferSize;
    u32 index;
} Arena;

Arena* arenaInit(u32 size = 2048);
void* arenaAlloc(u32 size, Arena* arena);

typedef struct StackAllocator {
    char* buffer;
    u32 bufferSize;
    u32 index;
} StackAllocator;

#endif

