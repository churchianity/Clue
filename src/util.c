
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void *pmalloc(unsigned int size) {
    void *p = malloc(size);

    if (!p) {
        fprintf(stderr, "out of memory... zzz...\n");
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

void** arrayGrow(void** array, unsigned int elemSizeInBytes, unsigned int count, unsigned int capacity) {
    if (capacity < count) {
        capacity *= 2;
        *array = realloc(*array, elemSizeInBytes * capacity);

        if (!array) {
            fprintf(stderr, "failed to realloc array. exiting...\n");
            exit(1);
        }
    }

    return array;
}
