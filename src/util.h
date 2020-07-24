
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "alloc.h"
#include "types.h"


extern u32 getFileSize(FILE* fp);
extern char* fileRead(const char* filename);


inline bool isDigit(char c) {
    return (c >= '0') && (c <= '9');
}

inline bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

inline bool isHexDigit(char c) {
    return ((c >= '0') && (c <= '9'))
        || ((c >= 'A') && (c <= 'F'))
        || ((c >= 'a') && (c <= 'f'));
}

inline bool isOctalDigit(char c) {
    return ((c >= '0') && (c <= '7'));
}

inline bool isBinaryDigit(char c) {
    return (c == '0') || (c == '1');
}

inline const char* boolToString(bool b) {
    return b ? "true" : "false";
}

inline char* intToString(u64 integer) {
    u32 capacity = 10;
    u32* remainders = (u32*) pMalloc(sizeof (u32) * capacity);

    u32 count = 0;

    while (true) {
        if (capacity <= count) {
            capacity *= 2;
            remainders = (u32*) pRealloc(remainders, sizeof (u32) * capacity);
        }

        remainders[count++] = integer % 10;

        integer /= 10;

        if (integer == 0) {
            break;
        }
    }

    char* buffer = (char*) pMalloc(sizeof (char) * count + 1);

    for (u32 i = 0; i < count; i++) {
        buffer[count - i - 1] = '0' + remainders[i];
    }

    buffer[count] = '\0';

    free(remainders);

    return buffer;
}

#endif

