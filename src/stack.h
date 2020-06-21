
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

/**
 *
 */
typedef struct Stack {
    unsigned int maxSize;
    unsigned int top;
    void** data;

    unsigned int (*size) (const struct Stack* self);
    bool (*isEmpty) (const struct Stack* self);
    bool (*isFull) (const struct Stack* self);
    void (*push) (struct Stack* self, void* dataItem);
    void* (*peek) (const struct Stack* self);
    void* (*pop) (struct Stack* self);
} Stack;

Stack* newStack(unsigned int capacity);

#endif

