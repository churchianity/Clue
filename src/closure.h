

#ifndef CLOSURE_H
#define CLOSURE_H

#include "table.hpp"
#include "value.h"


struct Closure {
    const char* name;
    Table<const char, Value>* table;
    Closure* parent;
};

#endif

