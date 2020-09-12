
#ifndef ALLOC_H
#define ALLOC_H

#include "types.h"


extern void* pMalloc(u32 size);
extern void* pCalloc(u32 elementSize, u32 maxNumOfElements);
extern void* pRealloc(void* buffer, u32 newSize);

#endif

