
#ifndef ARRAY_H
#define ARRAY_H

#include "alloc.h"
#include "types.h"


template <class T>
struct Array {
    u32 capacity;
    u32 length;
    T** data;

    Array<T>(u32 _capacity) {
        capacity = _capacity;
        length   = 0;
        data     = (T**) pCalloc(sizeof (T*), capacity);
    }

    bool isEmpty() const {
        return length == 0;
    }

    bool isFull() const {
        return length == capacity;
    }

    s32 push(T* dataItemAddr) {
        if (isFull()) {
            capacity *= 2;
            data = (T**) pRealloc(data, sizeof (T*) * capacity);
        }

        data[length++] = dataItemAddr;

        return 0;
    }

    Array<T> filter(bool (*predicate) (T*)) {
        Array<T> out = new Array<T>(length);

        for (u32 i = 0; i < length; i++) {
            if (predicate(data[i])) {
                out->push(data[i]);
            }
        }

        return out;
    }

    T* peek() const {
        if (isEmpty()) {
            return null;
        }

        return data[length - 1];
    }

    T* pop() {
        if (isEmpty()) {
            return null;
        }

        return data[--length];
    }

    void forEach(void (*callback) (T*)) {
        for (u32 i = 0; i < length; i++) {
            callback(data[i]);
        }
    }
};

#endif

