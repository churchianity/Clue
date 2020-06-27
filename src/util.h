
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

void *pmalloc(unsigned int size);
void *pcalloc(unsigned int maxNumOfElements, unsigned int elementSize);
char* boolToString(bool b);

// @TODO protected-array-insert/grow function

#endif
