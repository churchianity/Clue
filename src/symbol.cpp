
#include "clue.h"
#include "symbol.h"
#include "table.h"
#include "util.h"


/**
 * The symbols defined and inserted into the global table - initially -
 * represent all of the reserved identifiers and operators in the language.

Table* initGlobalSymbolTable() {
    Table *t = newTable(CLUE_GLOBAL_SYMBOL_TABLE_SIZE);

    // t->insert(t, "import", newSymbol("import", 0, true));
    // t->insert(t, "print", newSymbol("print", 0, true));

    return t;

}

*/
Symbol* newSymbol(const char* name, bool reserved) {

    Symbol* symbol = (Symbol*) pMalloc(sizeof (Symbol));

    symbol->name = name;
    symbol->reserved = reserved;
    // symbol->closure = closure ? closure : global;

    return symbol;
}

