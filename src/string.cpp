
#include <stdlib.h>

#include "alloc.h"
#include "types.h"

/**
 * @WARN Assumes null termination.
 */
u32 strln(const char* string) {
    u32 length = 0;

    while (*string != '\0') {
        length++;
        string++;
    }

    return length;
}

/**
 * @WARN Assumes null termination.
 */
bool streq(const char* s1, const char* s2) {
    if (strln(s1) != strln(s2)) {
        return false;
    }

    for (u32 i = 0; i < strln(s1); i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }

    return true;
}

const char* strcp(const char* string, u32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * (length + 1));

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = string[i];
    }

    buffer[i] = '\0';

    return buffer;
}

/**
 * like strrchr
 */
const char* lastCharOccurence(const char* string, u32 length, char c) {
    for (u32 i = length - 1; i >= 0; i--) {
        if (*(string + i) == c) {
            return string + i;
        }
    }

    return null;
}

/**
 * @WARN Assumes null termination.
 */
bool hasSuffix(const char* string, const char* suffix) {
    const char* p = lastCharOccurence(string, strln(string), suffix[0]);

    if (p) {
        return streq(p, suffix);
    }

    return false;
}

/**
 * @WARN Assumes null termination.
 */
u32 countLines(const char* buffer) {
    u32 lines = 0;
    char c;

    while ((c = *buffer) != '\0') {
        if (c == '\n') {
            lines++;
        }

        buffer++;
    }

    return lines;
}

