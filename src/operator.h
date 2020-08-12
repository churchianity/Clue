
#ifndef OPERATOR_H
#define OPERATOR_H

#include "table.hpp"


enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT = -1,
    OA_NONE = 0,
    OA_RIGHT_TO_LEFT = 1
};

struct Operator {
    u8 precedence;
    OperatorAssociativityEnum associativity = OA_LEFT_TO_RIGHT;
};

extern Table<const char, Operator>* OperatorTable;

#endif

