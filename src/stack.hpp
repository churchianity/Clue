
#ifndef STACK_H
#define STACK_H

#include "alloc.h"
#include "types.h"


template <class T>
struct Stack {
    u32 capacity;
    s32 top;
    bool grow;
    T** data;

    Stack<T>(u32 _capacity, bool _grow) {
        capacity = _capacity;
        grow     = _grow;
        top      = -1;
        data     = (T**) pMalloc(sizeof (T*) * capacity);
    }

    u32 size() const {
        return (u32) (top + 1); // 'top' is signed
    }

    bool isEmpty() const {
        return top == -1;
    }

    bool isFull() const {
        return ((u32) top) == (capacity - 1);
    }

    s32 push(T* dataItemAddr) {
        if (isFull()) {
            if (!grow) {
                return -1;
            }

            capacity *= 2;
            data = (T**) pRealloc(data, sizeof (T*) * capacity);
        }

        data[++top] = dataItemAddr;

        return 0;
    }

    T* peek() const {
        if (isEmpty()) {
            return null;
        }

        return data[top];
    }

    T* pop() {
        if (isEmpty()) {
            return null;
        }

        return data[top--];
    }

    void forEach(void (*callback) (const T*)) {
        for (u32 i = 0; i < size(); i++) {
            callback(data[i]);
        }
    }
};

#endif

