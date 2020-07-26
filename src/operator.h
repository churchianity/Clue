
#ifndef OPERATOR_H
#define OPERATOR_H

#include "types.h"


enum OperatorAssociativityEnum {
    OA_LEFT = -1,
    OA_RIGHT = 1
};

struct Operator {
    const char* name;

    OperatorAssociativityEnum associativity = OA_LEFT;

    u8 precedence = 0;

    bool unary    = false;
    bool postfix  = false;
    bool call     = false;
};

#endif

