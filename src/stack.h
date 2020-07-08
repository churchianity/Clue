
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#include "clue.h"


typedef struct Stack {
    u32 capacity;
    s32 top;
    bool grow;
    void** data;

    u32 (*size) (const struct Stack* self);
    bool (*isEmpty) (const struct Stack* self);
    bool (*isFull) (const struct Stack* self);
    s32 (*push) (struct Stack* self, void* dataItemAddr);
    void* (*peek) (const struct Stack* self);
    void* (*pop) (struct Stack* self);
    const char* (*toString) (struct Stack* self);
} Stack;

Stack* newStack(u32 capacity, bool grow);

#endif

