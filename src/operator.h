
#ifndef OPERATOR_H
#define OPERATOR_H

#include "table.h"


enum OperatorAssociativityEnum {
    OA_LEFT = -1,
    OA_RIGHT = 1
};

struct Operator {
    const char* name;

    OperatorAssociativityEnum associativity;

    u8 precedence;

    bool isUnary : 1;
    bool isPostfix : 1;
    bool isCall : 1;
};

extern u8 precedence(int tt, bool isUnary, bool isPostfix);
extern void initOperatorTable();

#endif

