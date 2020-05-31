
#include <stdio.h>
#include <stdlib.h>

void *pmalloc(size_t size) {
    void *p = malloc(size);

    if (!p) {
        fprintf(stderr, "out of memory... zzz...\n");
        exit(1);
    }

    return p;
}

