
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
    return self->top == self->maxSize - 1;
}

static void push(Stack* self, void* elem) {
    if (isFull(self)) {
        printf("stack 2 smol\n");
        exit(1);
    }

    self->data[++self->top] = elem;
}

static void* peek(const Stack* self) {
    if (isEmpty(self)) {
        printf("stack has no shit bro u aint peeking shit\n");
        exit(1);
    }

    return self->data[self->top];
}

static void* pop(Stack* self) {
    if (isEmpty(self)) {
        printf("stack has no shit bro u aint peeking shit\n");
        exit(1);
    }

    return self->data[self->top--];
}

Stack* newStack(unsigned int capacity) {
    Stack* stack = pmalloc(sizeof (Stack));

    stack->maxSize = capacity;
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

