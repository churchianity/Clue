

#ifndef ARRAY_H
#define ARRAY_H

#include <new> // operator new, operator delete

#include "alloc.h" // allocators...
#include "string.h" // memcpy
#include "types.h" // type definitions

#include "print.h"


template <class T>
struct Array {
    u32 capacity;
    u32 length;
    T** data;

    // if you know exactly how many elements will live in your array, you should always specify the capacity.
    // when adding elements with |push| or |unshift|, new space is allocated as required,
    // at a rate of 2x (because of amortized cost), leading to wasted space if you could have initially specified
    // the exact capacity.
    //
    // the internal array does not automatically shrink, ever.
    Array<T>(u32 _capacity = 10) {
        capacity = _capacity;
        length   = 0;
        data     = (T**) pCalloc(sizeof (T*), capacity);
    }

    void* operator new(size_t size) {
        return (Array<T>*) pMalloc(sizeof (Array<T>));
    }

    void operator delete(void* p) {
        pFree(p);
    }

    Array<T>* concat(Array<T> other) const {
        Array* array = new Array();

        this->forEach(
            [array] (T e) {
                array->push(e);
            }
        );

        other->forEach(
            [array] (T e) {
                array->push(e);
            }
        );

        return array;
    }

    // skip Array.prototype.copyWithin() - don't care
    // skip Array.prototype.entries() - don't know how to do iterators

    bool every(bool (*callback) (T*)) const {
        this->forEach(
            [callback] (T* e) {
                if (!callback(e)) {
                    return false;
                }
            }
        );

        return true;
    }

    Array<T>* fill(T* e) {
        for (u32 i = 0; i < this->length; i++) {
            if (this->data[i] != null) {
                pFree(this->data[i]);
            }

            this->data[i] = e;
        }

        return this;
    }

    Array<T>* filter(bool (*predicate) (T*)) {
        Array<T>* out = new Array<T>(length);

        for (u32 i = 0; i < this->length; i++) {
            if (predicate(this->data[i])) {
                out->push(this->data[i]);
            }
        }

        return out;
    }

    T* find(bool (*predicate) (T*)) const {
        for (u32 i = 0; i < this->length; i++) {
            if (predicate(this->data[i])) {
                return this->data[i];
            }
        }

        return null;
    }

    s32 findIndex(bool (*predicate) (T*)) const {
        for (u32 i = 0; i < this->length; i++) {
            if (predicate(this->data[i])) {
                return i;
            }
        }

        return -1;
    }

    // @TODO flat
    // @TODO flatMap

    void forEach(void (*callback) (T*)) const {
        for (u32 i = 0; i < length; i++) {
            callback(this->data[i]);
        }
    }

    bool includes(T* value) const {
        this->forEach(
            [value] (T* e) {
                if (value == e) {
                    return true;
                }
            }
        );

        return false;
    }

    s32 indexOf(T* value) const {
        for (u32 i = 0; i < this->length; i++) {
            if (value == this->data[i]) {
                return i;
            }
        }

        return -1;
    }

    // @TODO join() - makes no sense in arrays not of strings or if we don't have a concept of a 'toString()'

    u32* keys() {
        u32* array = pMalloc(sizeof(u32) * this->length);

        for (u32 i = 0; i < this->length; i++) {
            array[i] = i;
        }

        return array;
    }

    s32 lastIndexOf(T* value) const {
        for (u32 i = this->length - 1; i >= 0; i--) {
            if (value == this->data[i]) {
                return i;
            }
        }

        return -1;
    }

    Array<T>* map(void* (*callback) (T*)) const {
        Array<void>* array = new Array<void>();

        this->forEach(
            [array, callback] (T* e) {
                array->push(callback(e));
            }
        );

        return array;
    }

    T* pop() {
        if (this->isEmpty()) {
            return null;
        }

        return this->data[--this->length];
    }

    void push(T* e) {
        if (this->isFull()) {
            this->capacity *= 2;
            this->data = (T**) pRealloc(data, sizeof (T*) * this->capacity);
        }

        this->data[this->length++] = e;
    }

    // @TODO reduce(), reduceRight() - polymorphism and/or heavy overloading required on the return type

    Array<T>* reverse() {
        u32 count = this->length / 2;

        for (u32 i = 0; i < count; i++) {
            u32 offset = this->length - 1 - i;

            T* temp = this->data[i];
            this->data[i] = this->data[offset];
            this->data[offset] = temp;
        }

        return this;
    }

    T* shift() {
        if (this->length == 0) {
            return null;
        }

        T* out = this->data[0];
        this->length -= 1;

        for (u32 i = 0; i < this->length; i++) {
            *(this->data + i) = *(this->data + i + 1);
        }

        return out;
    }

    // @TODO test
    Array<T>* slice(u32 start = 0, u32 end = -1) {
        Array<T>* array = new Array();

        u32 length = end > this->length ? this->length : end;

        for (u32 i = start; i < length; i++) {
            Str :: memcpy(array->data[i], this->data[i], sizeof (T));
        }

        return array;
    }

    bool some(bool (*test) (T*)) {
        this->forEach(
            [test] (T* e) {
                if (test(e)) {
                    return true;
                }
            }
        );

        return false;
    }

    /* @TODO - refer to linux qsort
    Array<T>* sort(int (*comparator) (T*)) {

    }

    // @TODO
    Array<T>* splice(int start, int deleteCount, Array<T>* stuff) {
        return null;
    }

    // @TODO toLocaleString()
    // @TODO toSource()
    // @TODO toString()
    */

    // @TODO test
    T* unshift(T* e) {
        if (this->isFull()) {
            this->capacity *= 2;
            this->data = (T**) pRealloc(data, sizeof (T*) * this->capacity);
        }

        for (u32 i = 0; i < this->length; i++) {
            *(this->data + i + 1) = *(this->data + i);
        }

        this->data[0] = e;
        this->length += 1;

        return this->length;
    }

    T* values() {
        return this->data;
    }

    T* peek() const {
        if (this->isEmpty()) {
            return null;
        }

        return this->data[this->length - 1];
    }

    bool isEmpty() const {
        return this->length == 0;
    }

    bool isFull() const {
        return this->length == this->capacity;
    }
};

#endif

