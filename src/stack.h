
#ifndef STACK_H
#define STACK_H

#include "clue.h"


typedef struct Stack {
    u32 capacity;
    s32 top;
    bool grow;
    void** data;

    u32 (*size) (const Stack* self);
    bool (*isEmpty) (const Stack* self);
    bool (*isFull) (const Stack* self);
    s32 (*push) (Stack* self, void* dataItemAddr);
    void* (*peek) (const Stack* self);
    void* (*pop) (Stack* self);
} Stack;

extern Stack* newStack(u32 capacity, bool grow);

#endif

