
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

void *pmalloc(unsigned int size);
char* boolToString(bool b);
void** arrayGrow(void** array, unsigned int count, unsigned int capacity);

#endif
