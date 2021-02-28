
#ifndef ARRAY_H
#define ARRAY_H

#include "alloc.h" // allocators...
#include "types.h" // type definitions


/**
 * This is a growing Array, and a bunch of associated common/useful functions as methods.
 */
template <class T>
struct Array {
    u32 capacity;
    u32 length;
    T** data;

    Array<T>(u32 _capacity = 10) {
        capacity = _capacity;
        length   = 0;
        data     = (T**) pCalloc(sizeof (T*), capacity);
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
            // @NOTE/@TODO free existing stuff
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

    Array<T>* map(T* (*callback) (T*)) const {
        Array array = new Array();

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

    void push(T* dataItemAddr) {
        if (this->isFull()) {
            this->capacity *= 2;
            this->data = (T**) pRealloc(data, sizeof (T*) * this->capacity);
        }

        this->data[this->length++] = dataItemAddr;
    }

    // @TODO reduce(), reduceRight() - polymorphism and/or heavy overloading required on the return type

    Array<T>* reverse() {
        for (u32 i = 0; i < this->length; i++) {
            T* temp = this->data[i];
            this->data[i] = this->data[this->length - 1 - i];
            this->data[this->length - 1 - i] = temp;
        }

        return this;
    }

    // @TODO
    T* shift() {
        if (this->length == 0) {
            return null;
        }

        const auto out = this->data[0];

        for (u32 i = 0; i != sizeof (this) - 1; i++) {
            *(this->data + i) = *(this->data + i + 1);
        }

        return out;
    }

    Array<T>* slice(u32 start, u32 end = 0) {
        Array<T>* array = new Array();

        u32 length = end > this->length ? this->length : end;

        for (u32 i = start; i < length; i++) {
            array->push(this->data[i]);
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

    /* @TODO, gets spicy with algorithms and choice there, prolly quicksort
    Array<T>* sort(int (*comparator) (T*)) {

    }

    // @TODO
    Array<T>* splice(int start, int deleteCount, Array<T>* stuff) {
        return null;
    }

    // @TODO toLocaleString()
    // @TODO toSource()
    // @TODO toString()

    // @TODO
    T* unshift(Array<T>* stuff) {
        return null;
    }

    // @TODO probably shouldn't use this
    T* values() {
        return this->data;
    }
    */

    // non-standard
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

