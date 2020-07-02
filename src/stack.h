
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#include "clue.h"

typedef struct Stack {
    u64 capacity;
    s64 top;
    bool grow;
    void** data;

    u64 (*size) (const struct Stack* self);
    bool (*isEmpty) (const struct Stack* self);
    bool (*isFull) (const struct Stack* self);
    s64 (*push) (struct Stack* self, void* dataItemAddr);
    void* (*peek) (const struct Stack* self);
    void* (*pop) (struct Stack* self);
    char* (*toString) (struct Stack* self);
} Stack;

Stack* newStack(u64 capacity, bool grow);

#endif

