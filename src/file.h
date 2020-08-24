
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "alloc.h"
#include "print.h"
#include "types.h"


/**
 * @TODO @FIXME these need to be using the windows api & unistd.h instead of stdio.h
 */
inline u32 getFileSize(FILE* fp) {
    s32 size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
        die("ftell returned a negative value: %d\n", (s32) size);
    }

    return (u32) size;
}

/**
 * Internally allocates the returned buffer - caller responsible for free'ing.
 */
inline char* fileRead(const char* filePath) {
    FILE* fp = fopen(filePath, "r");

    if (!fp) {
        die("failed to get fp for filepath: %s\n", filePath);
    }

    u32 length = getFileSize(fp);

    char* buffer = (char*) pMalloc(length);

    fread(buffer, sizeof (char), length, fp);
    fclose(fp);

    return buffer;
}

inline char* clueFileRead(const char* filePath) {
    // @TODO search the filesystem!
    // assume the path is correct, but if the file is not found, make a series of educated guesses and try again!

    // Guesses:
    // 1. the source is all under some folder with some name
    // probably, 'src'
    //
    // 2. the source is all under a folder that is a git or svn repository
    //
    // 3. heuristic: some number of filesystem ascensions is probably a good indicator we just can't find it
    //
    // ... ? do we do a spellchecker?
    FILE* fp = fopen(filePath, "r");

    if (!fp) {
        die("failed to get fp for filepath: %s\n", filePath);
    }

    u32 length = getFileSize(fp);

    char* buffer = (char*) pMalloc(length);

    fread(buffer, sizeof (char), length, fp);
    fclose(fp);

    if (isAscii(buffer, length)) {
        print("loaded file %s, as 7-bit ASCII.\n", filePath);

    } else {
        print("loaded file %s, unknown encoding.\n", filePath);
    }

    return buffer;
}

#endif

