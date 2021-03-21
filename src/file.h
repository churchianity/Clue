
#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "alloc.h"
#include "types.h"

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

