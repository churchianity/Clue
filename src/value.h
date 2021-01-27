
#ifndef VALUE_H
#define VALUE_H


enum ValueTypeEnum {
    VT_NUMBER,
    VT_STRING,
    VT_POINTER
};

struct Value {
    ValueTypeEnum type;

    union {
        double number;
        const char* string;
        void* pointer;
    };
};

#endif

