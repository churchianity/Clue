
#include "symbol.h"
#include "util.h"

Symbol* newSymbol(const char* name, unsigned int precedence, bool reserved) {
    Symbol* symbol = pMalloc(sizeof (Symbol));

    symbol->name = name;
    symbol->precedence = precedence;
    symbol->reserved = reserved;

    return symbol;
}

