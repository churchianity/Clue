
#ifndef SYMBOL_H
#define SYMBOL_H

#include "clue.h"


enum OperatorAssociativityEnum {
    OA_LEFT = -1,
    OA_RIGHT = 1
};

struct Symbol {
    bool reserved;
    OperatorAssociativityEnum associativity;
    const char* name;
    char precedence;
};

extern Symbol* newSymbol(const char* name, u8 precedence, bool reserved);

#endif

