
#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "alloc.h"
#include "print.h"
#include "string.h"
#include "types.h"


enum FileFlags {
    FF_HAS_CARRIAGE_RETURN = 0, // has atleast one '\r'
    FF_HAS_REGULAR_NEWLINE = 1, // has atleast one '\n' without a preceding '\r'
    FF_HAS_TABS            = 2, // has atleast one '\t'
    FF_HAS_SPACES          = 4, // has atleast one instance of 4 consecutive spaces, or has no tabs
    FF_IS_ASCII            = 8, // has no bytes with the 8th (most significant) bit set to 1
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
    u32 size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* buffer = (char*) pMalloc(size + 1);
    fread(buffer, sizeof (char), size + 1, fp);
    buffer[size] = '\0';

    fclose(fp);

    if (isAscii(buffer, size)) {
        print("loaded file %s, as 7-bit ASCII.\n", filePath);

    } else {
        print("loaded file %s, unknown encoding.\n", filePath);
    }

    return buffer;
}

#endif

