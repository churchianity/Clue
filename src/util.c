
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void *pmalloc(unsigned int size) {
    void *p = malloc(size);

    if (!p) {
        fprintf(stderr, "failed to malloc... zzz...\n");
        exit(1);
    }

    return p;
}

void *pcalloc(unsigned int maxNumOfElements, unsigned int elementSize) {
    void *p = calloc(maxNumOfElements, elementSize);

    if (!p) {
        fprintf(stderr, "failed to calloc... zzz...\n");
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

