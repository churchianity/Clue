
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

void *pMalloc(unsigned int size);
void *pCalloc(unsigned int maxNumOfElements, unsigned int elementSize);
void *pRealloc(void* buffer, unsigned int newSize);

char* boolToString(bool b);
bool hasSuffix(const char* string, const char* suffix);
unsigned long getFileSize(FILE* fp);
unsigned long countLines(const char* buffer);

#endif

