
#ifndef RUNTIME_H
#define RUNTIME_H

#include "clue.h"


enum ValueTypeEnum {
    VT_STRING,
    VT_NUMBER
};

struct Value {
    ValueTypeEnum type = VT_NUMBER;

    union {
        const char* string;
        float64 number;
    };
};

extern Value eval(ASTNode* node);
extern void interactive();
extern void doIt(char* codeBuffer, const char* filename);

#endif

