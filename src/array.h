
#ifndef ARRAY_H
#define ARRAY_H

template <class T>
struct Array {
    u32 capacity;
    u32 length;
    T** data;
};

#endif

