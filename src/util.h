
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "alloc.h"
#include "types.h"


extern u32 getFileSize(FILE* fp);
extern char* fileRead(const char* filePath);
extern char* clueFileRead(const char* filePath);

#endif

