
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
bool streq(const char* s1, const char* s2);
bool hasSuffix(const char* string, const char* suffix);
unsigned long countLines(const char* buffer);

unsigned long getFileSize(FILE* fp);
char* fileRead(const char* filename);


#endif

