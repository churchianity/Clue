
#ifndef SCOPE_H
#define SCOPE_H

#include "table.hpp"
#include "value.h"

typedef struct Scope {
    const char* name;
    Table<const char, Value>* table;
    Scope* parent;
} Scope;

#endif

