
#ifndef VALUE_H
#define VALUE_H

#include "types.h"


enum ValueTypeEnum {
    VT_NUMBER,
    VT_STRING
};

struct Value {
    ValueTypeEnum type;

    union {
        double number;
        const char* string;
    };
};

#endif

