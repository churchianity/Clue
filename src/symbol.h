
#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>

typedef struct {
    bool reserved;
    const char* name;
    unsigned int precedence;
} Symbol;

Symbol* newSymbol(const char* name, unsigned int precedence, bool reserved);

#endif
