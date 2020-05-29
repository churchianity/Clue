
#include <stdio.h>
#include <stdlib.h>

void *pmalloc(size_t size) {
    void *p = malloc(size);

    if (!p) {
        printf("out of memory... zzz...\n");
        exit(1);
    }

    return p;
}

