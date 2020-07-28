
#include <stdlib.h>

#include "clue.h"
#include "print.h"
#include "string.h"
#include "util.h"


u32 getFileSize(FILE* fp) {
    u32 size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
        die("ftell returned a negative value: %d\n", (s32) size);
    }

    return size;
}

/**
 * Internally allocates the returned buffer - caller responsible for free'ing.
 */
char* fileRead(const char* filename) {
    FILE* fp = fopen(filename, "r");

    if (!fp) {
        die("failed to get fp for filename: %s\n", filename);
    }

    u32 length = getFileSize(fp);

    char* buffer = (char*) pMalloc(length);

    fread(buffer, 1, length, fp);

    if (isAscii(buffer, length)) {
        printf("loaded file %s, as 7-bit ASCII.\n", filename);

    } else {
        printf("loaded file %s, as unknown encoding.\n", filename);
    }

    fclose(fp);

    return buffer;
}

