
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"
#include "util.h"

static unsigned int size(const Stack* stack) {
    return stack->top + 1;
}

static bool isEmpty(const Stack* stack) {
    return stack->top == -1;
}

static bool isFull(const Stack* stack) {
    return stack->top == stack->maxSize - 1;
}

static void push(Stack* stack, void* elem) {
    if (isFull(stack)) {
        printf("stack 2 smol\n");
        exit(1);
    }

    stack->data[++stack->top] = elem;
}

static void* peek(const Stack* stack) {
    if (isEmpty(stack)) {
        printf("stack has no shit bro u aint peeking shit\n");
        exit(1);
    }

    return stack->data[stack->top];
}

static void* pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("stack has no shit bro u aint peeking shit\n");
        exit(1);
    }

    return stack->data[stack->top--];
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

