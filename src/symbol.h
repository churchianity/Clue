
#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>

#include "clue.h"


typedef enum {
    OA_LEFT = -1,
    OA_RIGHT = 1
} OperatorAssociativityEnum;

typedef struct {
    bool reserved;
    const char* name;
    char precedence;
} Symbol;

Symbol* newSymbol(const char* name, char precedence, bool reserved);

#endif

