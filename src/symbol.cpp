
#include "clue.h"
#include "symbol.h"
#include "table.hpp"
#include "util.h"


Symbol* newSymbol(const char* name, bool reserved) {

    Symbol* symbol = (Symbol*) pMalloc(sizeof (Symbol));

    symbol->name = name;
    symbol->reserved = reserved;
    // symbol->closure = closure ? closure : global;

    return symbol;
}

