
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

#include "clue.h"


void* pMalloc(u32 size);
void* pCalloc(u32 maxNumOfElements, u32 elementSize);
void* pRealloc(void* buffer, u32 newSize);

char* boolToString(bool b);

u64 strln(const char* string);
bool streq(const char* s1, const char* s2);
bool hasSuffix(const char* string, const char* suffix);
unsigned long countLines(const char* buffer);

unsigned long getFileSize(FILE* fp);
char* fileRead(const char* filename);


#endif

