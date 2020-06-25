
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

static signed int push(Stack* self, void* elem) {
    if (isFull(self)) {
        if (!self->grow) {
            return -1;
        }

        self->capacity *= 2;
        self->data = realloc(self->data, sizeof (void*) * self->capacity);
    }

    self->data[++self->top] = elem;
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

Stack* newStack(unsigned int capacity, bool grow) {
    Stack* stack = pmalloc(sizeof (Stack));

    stack->capacity = capacity;
    stack->grow = grow;
    stack->top = -1;
    stack->data = pmalloc(sizeof (void*) * capacity);

    stack->size = &size;
    stack->isEmpty = &isEmpty;
    stack->isFull = &isFull;
    stack->push = &push;
    stack->peek = &peek;
    stack->pop = &pop;

    return stack;
}

