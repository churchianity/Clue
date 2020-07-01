
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"
#include "util.h"

static unsigned int size(const Stack* self) {
    return self->top + 1;
}

static bool isEmpty(const Stack* self) {
    return self->top == -1;
}

static bool isFull(const Stack* self) {
    return self->top == self->capacity - 1;
}

static signed int push(Stack* self, void* dataItemAddr) {
    if (isFull(self)) {
        if (!self->grow) {
            return -1;
        }

        self->capacity *= 2;
        self->data = pRealloc(self->data, sizeof (void*) * self->capacity);
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

/**
 * @TODO this function is unsafe
 */
static char* toString(Stack* self) {
    const char* grow = boolToString(self->grow);

    const char* format = "capacity: %d, grow?: %s, top: %d, size: %d, data: %p\n";

    // magic numbers are assumed lengths as strings of properties after being format specified
    // @TODO fix
    const unsigned int length = 4 + strlen(grow) + 4 + 4 + 14 + strlen(format);

    char* buffer = pMalloc(length + 1);

    snprintf(buffer, length, format
            , self->capacity
            , grow
            , self->top
            , self->size(self)
            , *self->data
    );

    return buffer;
}

Stack* newStack(unsigned int capacity, bool grow) {
    Stack* stack = pMalloc(sizeof (Stack));

    stack->capacity = capacity;
    stack->grow = grow;
    stack->top = -1;
    stack->data = pMalloc(sizeof (void*) * capacity);

    stack->size = &size;
    stack->isEmpty = &isEmpty;
    stack->isFull = &isFull;
    stack->push = &push;
    stack->peek = &peek;
    stack->pop = &pop;
    stack->toString = &toString;

    return stack;
}

