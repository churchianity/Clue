
#ifndef VALUE_H
#define VALUE_H

#include "types.h"

enum ValueTypeEnum {
    VT_NUMBER,
    VT_STRING
};

struct Value {
    ValueTypeEnum type = VT_NUMBER;

    union {
        float64 number = 0xDEADBEEF;
        const char* string;
    };
};

#endif

