
#ifndef RUNTIME_H
#define RUNTIME_H

#include "clue.h"


union Value {
    const char* string;
    float64 number;
};

extern Value eval(ASTNode* node);
extern void clueFileRead(const char* filename);
extern void interactive();

#endif

