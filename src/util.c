
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "util.h"

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
