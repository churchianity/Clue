
#include <stdio.h>
#include <stdlib.h>

void *pmalloc(unsigned int size) {
    void *p = malloc(size);

    if (!p) {
        fprintf(stderr, "out of memory... zzz...\n");
        exit(1);
    }

    return p;
}

