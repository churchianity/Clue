
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

#include "clue.h"


u32 strln(const char* string);
bool streq(const char* s1, const char* s2);
bool hasSuffix(const char* string, const char* suffix);
u32 countLines(const char* buffer);

u32 getFileSize(FILE* fp);
char* fileRead(const char* filename);


inline void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        fprintf(stderr, "failed to malloc... zzz...\n");
        exit(1);
    }

    return p;
}

inline void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        fprintf(stderr, "failed to calloc... zzz...\n");
        exit(1);
    }

    return p;
}

inline void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        fprintf(stderr, "failed to realloc... zzz...\n");
        exit(1);
    }

    return p;
}

inline bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

inline bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

inline const char* boolToString(bool b) {
    return b ? "true" : "false";
}

inline bool isAscii(const char* buffer, u32 length) {
    const unsigned char* ubuffer = (const unsigned char*) buffer;

    for (u32 i = 0; i < length; i++) {
        if (ubuffer[i] & 0b10000000) {
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

inline char* concat(char* str1, char* str2) {
    u32 l1 = strln(str1);
    u32 l2 = strln(str2);

    char* out = (char*) pMalloc(sizeof (char) * (l1 + l2) + 1);

    u32 i = 0;
    for (; i < l1; i++) {
        out[i] = str1[i];
    }

    for (; i < l2; i++) {
        out[i] = str2[i];
    }

    out[i] = '\0';

    return out;
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

