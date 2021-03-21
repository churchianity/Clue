
#ifndef TYPES_H
#define TYPES_H

// these are the internal, c/c++ types used by this implementation of the compiler/interpreter/thing
// , not the complete list of types used by the language itself

#define null 0

// bool is included by default for C++11
#ifndef __cplusplus
    typedef _Bool bool;
    #define true 1
    #define false 0
#endif

inline const char* boolToString(bool b) {
    return b ? "true" : "false";
}

// char // i don't know if we should use char8_t, seems like a medium sized pain for a small sized gain

#include <stdint.h> // required on my chromebook for some reason
                    // @NOTE chrome gcc -v 8.3.0   | maybe gcc version is the cause - im using the same flags tho

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

// typedef float float;
// typedef double double;
// typedef long double extended;

#endif

