
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void* pMalloc(unsigned int size) {
    void* p = malloc(size);

    if (!p) {
        fprintf(stderr, "failed to malloc... zzz...\n");
        exit(1);
    }

    return p;
}

void* pCalloc(unsigned int maxNumOfElements, unsigned int elementSize) {
    void* p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        fprintf(stderr, "failed to calloc... zzz...\n");
        exit(1);
    }

    return p;
}

void* pRealloc(void* buffer, unsigned int newSize) {
    void* p = realloc(buffer, newSize);

    if (!p) {
        fprintf(stderr, "failed to realloc... zzz.\n");
        exit(1);
    }

    return p;
}

char* boolToString(bool b) {
    if (b) {
        return "true";
    }

    return "false";
}

bool hasSuffix(char* string, char* suffix) {
    string = strrchr(string, suffix[0]);

    if (string) {
        return strcmp(string, suffix) == 0;
    }

    return false;
}

long getFileSize(FILE* fp) {
    long size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "ftell returned a negative value: %ld\n", size);
        exit(1);
    }

    return size;
}

unsigned long countLines(const char* buffer) {
    unsigned long lines = 0;
    char c;

    while ((c = *buffer) != '\0') {
        if (c == '\n') {
            lines++;
        }

        ++buffer;
    }

    return lines;
}

