
#ifndef SYMBOL_H
#define SYMBOL_H

#include "clue.h"


struct Symbol {
    const char* name;
    bool reserved;
};

extern Symbol* newSymbol(const char* name, void* value, bool reserved /*, scope*/);

#endif

