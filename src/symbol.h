
#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>

#include "clue.h"

typedef struct {
    bool reserved;
    const char* name;
    u64 precedence;
} Symbol;

Symbol* newSymbol(const char* name, u64 precedence, bool reserved);

#endif
