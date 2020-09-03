
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "alloc.h"
#include "print.h"
#include "string.h"
#include "types.h"

enum FileFlags {
    FF_HAS_CARRIAGE_RETURN,
    FF_HAS_TABS
};

struct FileInfo {
    u8 flags;
    u32 size;
};

inline char* clueFileRead(const char* filePath) {
    FILE* fp = fopen(filePath, "rb");

    if (fp == null) {
        // @TODO series of fallback filesystem searches
        die("couldn't open file %s, is the path correct?\n", filePath);
    }

    fseek(fp, 0, SEEK_END);
    u32 length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* buffer = (char*) pMalloc(length + 1);
    fread(buffer, sizeof (char), length + 1, fp);
    buffer[length] = '\0';

    fclose(fp);

    if (isAscii(buffer, length)) {
        print("loaded file %s, as 7-bit ASCII.\n", filePath);

    } else {
        print("loaded file %s, unknown encoding.\n", filePath);
    }

    return buffer;
}

#endif

