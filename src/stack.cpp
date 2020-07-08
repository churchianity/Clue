
#include <stdbool.h>
#include <stdio.h>

#include "stack.h"
#include "util.h"


static u32 size(const Stack* self) {
    return (u32) (self->top + 1); // 'top' is signed
}

static bool isEmpty(const Stack* self) {
    return self->top == -1;
}

static bool isFull(const Stack* self) {
    return self->top == self->capacity - 1;
}

static s32 push(Stack* self, void* dataItemAddr) {
    if (isFull(self)) {
        if (!self->grow) {
            return -1;
        }

        self->capacity *= 2;
        self->data = (void**) pRealloc(self->data, sizeof (void*) * self->capacity);
    }

    self->data[++self->top] = dataItemAddr;
    return 0;
}

static void* peek(const Stack* self) {
    if (isEmpty(self)) {
        return NULL;
    }

    return self->data[self->top];
}

static void* pop(Stack* self) {
    if (isEmpty(self)) {
        return NULL;
    }

    return self->data[self->top--];
}

Stack* newStack(u32 capacity, bool grow) {
    Stack* stack = (Stack*) pMalloc(sizeof (Stack));

    stack->capacity = capacity;
    stack->grow = grow;
    stack->top = -1;
    stack->data = (void**) pMalloc(sizeof (void*) * capacity);

    stack->size = &size;
    stack->isEmpty = &isEmpty;
    stack->isFull = &isFull;
    stack->push = &push;
    stack->peek = &peek;
    stack->pop = &pop;

    return stack;
}

