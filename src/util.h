
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

#include "clue.h"

void* pMalloc(u64 size);
void* pCalloc(u64 maxNumOfElements, u64 elementSize);
void* pRealloc(void* buffer, u64 newSize);

char* boolToString(bool b);

u64 strln(const char* string);
bool hasSuffix(const char* string, const char* suffix);
bool streq(const char* s1, const char* s2);

unsigned long getFileSize(FILE* fp);
unsigned long countLines(const char* buffer);

#endif

