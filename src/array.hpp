
#ifndef ARRAY_H
#define ARRAY_H

#include "alloc.h" // allocators...
#include "types.h" // type definitions


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
            [] (T e) {
                array->push(e);
            }
        );

        other->forEach(
            [] (T e) {
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
        for (int i = 0; i < this->length; i++) {
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
        this->forEach(
            [predicate] (T* e) {
                if (predicate(e)) {
                    return e;
                }
            }
        );

        return null;
    }

    int findIndex(bool (*predicate) (T*)) const {
        for (int i = 0; i < this->length; i++) {
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

    int indexOf(T* value) const {
        for (int i = 0; i < this->length; i++) {
            if (value == this->data[i]) {
                return i;
            }
        }

        return -1;
    }

    // @TODO join() - makes no sense in arrays not of strings or if we don't have a concept of a 'toString()'

    unsigned int* keys() {
        unsigned int* array = pMalloc(sizeof(unsigned int) * this->length);

        for (int i = 0; i < this->length; i++) {
            array[i] = i;
        }

        return array;
    }

    int lastIndexOf(T* value) const {
        for (int i = this->length - 1; i >= 0; i--) {
            if (value == this->data[i]) {
                return i;
            }
        }

        return -1;
    }

    Array<T>* map(T* (*callback) (T*)) const {
        Array array = new Array();

        this->forEach(
            [callback] (T* e) {
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
        for (int i = 0; i < this->length; i++) {
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
    }

    Array<T>* slice(unsigned int start, unsigned int end) {
        Array<T>* array = new Array();

        unsigned int length = end > this->length ? this->length : end;

        for (int i = start; i < length; i++) {
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

    Array<T>* sort(int (*comparator) (T*)) {

    }

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

