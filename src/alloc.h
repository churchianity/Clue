
#ifndef ALLOC_H
#define ALLOC_H

#include "types.h"


void* pMalloc(u32 size);
void* pCalloc(u32 elementSize, u32 maxNumOfElements);
void* pRealloc(void* buffer, u32 newSize);
void pFree(void* ptr);

#endif

