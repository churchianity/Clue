
#ifndef SYMBOL_H
#define SYMBOL_H

#include "clue.h"
#include "table.h"


struct Symbol {
    const char* name;
    bool reserved;
};

extern Symbol* newSymbol(const char* name, void* value, bool reserved /*, scope*/);
// extern Table* initGlobalSymbolTable();

#endif

