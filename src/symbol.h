
#ifndef SYMBOL_H
#define SYMBOL_H

#include "clue.h"


typedef enum {
    OA_LEFT = -1,
    OA_RIGHT = 1
} OperatorAssociativityEnum;

typedef struct {
    bool reserved;
    OperatorAssociativityEnum associativity;
    const char* name;
    char precedence;
} Symbol;

extern Symbol* newSymbol(const char* name, u8 precedence, bool reserved);

#endif

