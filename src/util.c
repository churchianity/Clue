
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strrchar??????

#include "clue.h"


void* pMalloc(u32 size) {
    void* p = malloc(size);

    if (!p) {
        fprintf(stderr, "failed to malloc... zzz...\n");
        exit(1);
    }

    return p;
}

void* pCalloc(u32 maxNumOfElements, u32 elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        fprintf(stderr, "failed to calloc... zzz...\n");
        exit(1);
    }

    return p;
}

void* pRealloc(void* buffer, u32 newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        fprintf(stderr, "failed to realloc... zzz.\n");
        exit(1);
    }

    return p;
}

char* boolToString(bool b) {
    if (b) {
        return (char*) "true";
    }

    return (char*) "false";
}

/*
bool isdigit(char c) {
    return (c >= '0') && (c <= '9');
}

bool isalpha(char c) {
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}
*/

// @NOTE it feels like c strings are trash, but it's barely not worth it to write a string lib myself, that could change
// when could I get a char* that isn't null terminated?

/**
 * @WARN Assumes null termination.
 */
u32 strln(const char* string) {
    u32 length = 0;

    while (*string != '\0') {
        ++length;
        ++string;
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

    for (u64 i = 0; i < strln(s1); ++i) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }

    return true;
}

/**
 * @WARN Assumes null termination.
 */
bool hasSuffix(const char* string, const char* suffix) {
    char* p = (char*) strrchr((char*) string, suffix[0]);

    if (p) {
        return streq(p, suffix);
    }

    return false;
}

u64 getFileSize(FILE* fp) {
    u64 size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "ftell returned a negative value: %llu\n", size);
        exit(1);
    }

    return size;
}

/**
 * @WARN Assumes null termination.
 */
u64 countLines(const char* buffer) {
    u64 lines = 0;
    char c;

    while ((c = *buffer) != '\0') {
        if (c == '\n') {
            lines++;
        }

        ++buffer;
    }

    return lines;
}

