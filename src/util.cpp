
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strrchar??????

#include "clue.h"
#include "util.h"


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

/* @TODO replace strrchr with this and make a string struct
char* lastCharOccurence(char* string, u32 length, char c) {
    for (u32 i = length - 1; i >= 0; i--) {
        if (*string + i == c) {
            return *string + i;
        }
    }

    return NULL;
}
*/

/**
 * @WARN Assumes null termination.
 */
bool hasSuffix(const char* string, const char* suffix) {
    const char* p = strrchr(string, suffix[0]);

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


u32 getFileSize(FILE* fp) {
    u32 size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "ftell returned a negative value: %d\n", (s32) size);
        exit(1);
    }

    return size;
}


/**
 * Internally allocates the returned buffer - caller responsible for free'ing.
 */
char* fileRead(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char* buffer = NULL;
    u32 length;

    if (!fp) {
        fprintf(stderr, "failed to get fp for filename: %s\n", filename);
        exit(1);
    }

    length = getFileSize(fp);
    buffer = (char*) pMalloc(length);

    fread(buffer, 1, length, fp);
    fclose(fp);

    return buffer;
}

