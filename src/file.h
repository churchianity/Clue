
#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "alloc.h"
#include "print.h"
#include "reporter.h"
#include "string.h"
#include "types.h"


/*
// @NOTE, probably not worth it for now to try and be smart about detecting various things like tabs vs. spaces
// none of this really matters as far as I can tell (except the ascii flag but we can just detect that by finding bad codepoints later)
enum FileFlags {
    FF_HAS_CARRIAGE_RETURN = 1, // has atleast one '\r'
    FF_HAS_REGULAR_NEWLINE = 2, // has atleast one '\n' without a preceding '\r'
    FF_HAS_TABS            = 4, // has atleast one '\t'
    FF_HAS_SPACES          = 8, // has atleast one instance of 4 consecutive spaces, or has no tabs
    FF_IS_ASCII            = 16, // has no bytes with the 8th (most significant) bit set to 1
};

struct FileInfo {
    u8 flags;
    u32 size;
};
*/

inline char* clueFileRead(const char* filePath) {
    FILE* fp = fopen(filePath, "rb");

    if (fp == null) {
        // @TODO series of fallback filesystem searches
        return null;
    }

    // get the file's size in bytes
    fseek(fp, 0, SEEK_END);
    u32 size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* buffer = (char*) pMalloc(size + 1);
    fread(buffer, sizeof (char), size + 1, fp);
    buffer[size] = '\0';

    fclose(fp);

    return buffer;
}

#endif

