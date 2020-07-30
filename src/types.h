
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>


typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

// there isn't a really good reason for this
#define null NULL

// bool - true, false
// char // i don't know if we should use char8_t, seems like a medium sized pain for a small sized gain
// float, double, extended. probably only double.
// string? probably not - see string.h

#endif

