
#ifndef VALUE_H
#define VALUE_H

#include "types.h"

enum ValueTypeEnum {
    VT_STRING,
    VT_NUMBER
};

struct Value {
    ValueTypeEnum type = VT_NUMBER;

    union {
        float64 number;
        const char* string;
    };
};

#endif

