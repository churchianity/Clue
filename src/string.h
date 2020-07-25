
#ifndef STRING_H
#define STRING_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"


extern u32 strln(const char* string);
extern bool streq(const char* s1, const char* s2);
extern bool hasSuffix(const char* string, const char* suffix);
extern u32 countLines(const char* buffer);

/**
 * Ascii encoded text won't ever set the 8th bit of any of its bytes.
 * @TODO is |length| necessary here?
 */
inline bool isAscii(const char* buffer, u32 length) {
    const unsigned char* ubuffer = (const unsigned char*) buffer;

    for (u32 i = 0; i < length; i++) {
        if (ubuffer[i] & 128) { // binary: 0b 1000 0000
            return false;
        }
    }

    return true;
}

inline char* trimQuotes(const char* str, u32 length) {
    if (length < 2) {
        fprintf(stderr, "trying to trim quotes off string smaller than 2 in length\n");
        exit(1);
    }

    char* buffer = (char*) pMalloc(sizeof (char) * (length - 1));

    u32 i = 0;
    for (; i < (length - 2); i++) {
        buffer[i] = str[i + 1];
    }

    buffer[i] = '\0';

    return buffer;
}

inline char* concat(const char* str1, const char* str2) {
    u32 l1 = strln(str1);
    u32 l2 = strln(str2);

    u32 newLength = l1 + l2;

    char* newBuffer = (char*) pMalloc(sizeof (char) * newLength + 1);

    u32 i = 0;
    for (; i < newLength; i++) {
        if (i < l1) {
            newBuffer[i] = str1[i];

        } else {
            newBuffer[i] = str2[i - l1];
        }
    }

    newBuffer[i] = '\0';

    return newBuffer;
}

inline char* concat(u32 argc, ...) {
    va_list args;
    va_start(args, argc);

    char* str1 = va_arg(args, char*);
    char* str2 = null;

    for (u32 i = 1; i < argc; i++) {
        str2 = va_arg(args, char*);

        str1 = concat(str1, str2);
    }

    va_end(args);

    return str1;
}

/**
 * Reads |length| characters from |buffer| into a newly allocated buffer and returns it.
 * Appends the null character, so the returned string is |length| + 1 in size.
 */
inline char* read(char* buffer, u32 length) {
    char* tk = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    while (i < length) {

        tk[i] = *(buffer + i);

        i++;
    }

    tk[i] = '\0';

    return tk;
}

#endif

