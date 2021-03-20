
#ifndef ALLOC_H
#define ALLOC_H

#include "types.h"


void* pMalloc(u32 size);
void* pCalloc(u32 elementSize, u32 maxNumOfElements = 1);
void* pRealloc(void* buffer, u32 newSize);
void pFree(void* ptr);
void pFreeConst(const void* ptr);

#endif

