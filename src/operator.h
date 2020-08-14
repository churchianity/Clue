
#ifndef OPERATOR_H
#define OPERATOR_H

#include "table.hpp"
#include "token.h"


enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT    = -1,
    OA_NONE             =  0,
    OA_RIGHT_TO_LEFT    =  1
};

struct Operator {
    u8 precedence = 8;
    OperatorAssociativityEnum associativity = OA_LEFT_TO_RIGHT;
    u32 type; // heavily overlaps with TokenTypeEnum
};

extern Table<const char, Operator>* OperatorTable;
extern void initOperatorTable();

#endif

