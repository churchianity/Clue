
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

#include "clue.h"


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

inline const char* boolToString(bool b) {
    return (b) ? "true" : "false";
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

u32 strln(const char* string);
bool streq(const char* s1, const char* s2);
bool hasSuffix(const char* string, const char* suffix);
u32 countLines(const char* buffer);

u32 getFileSize(FILE* fp);
char* fileRead(const char* filename);


#endif

