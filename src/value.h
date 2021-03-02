
#ifndef VALUE_H
#define VALUE_H


enum ValueTypeEnum {
    VT_INTEGER,
    VT_NUMBER,
    VT_STRING,
    VT_POINTER
};

struct Value {
    ValueTypeEnum type;

    union {
        s64 integer;
        double number;
        const char* string;
        void* pointer;
    };
};

#endif

