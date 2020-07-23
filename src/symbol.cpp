
#include "clue.h"
#include "symbol.h"
#include "table.h"
#include "util.h"


/**
 * The symbols defined and inserted into the global table - initially -
 * represent all of the reserved identifiers and operators in the language.
 */
Table* initGlobalSymbolTable() {
    Table *t = newTable(CLUE_GLOBAL_SYMBOL_TABLE_SIZE);

    t->insert(t, ";", newSymbol(";", 0, true));
    t->insert(t, ",", newSymbol(",", 0, true));
    t->insert(t, "(", newSymbol("(", 90, true));
    t->insert(t, ")", newSymbol(")", 90, true));
    t->insert(t, "{", newSymbol("{", 0, true));
    t->insert(t, "}", newSymbol("}", 90, true));
    t->insert(t, "[", newSymbol("[", 90, true));
    t->insert(t, "]", newSymbol("]", 90, true));

    t->insert(t, "=", newSymbol("=", 10, true));
    t->insert(t, ":=", newSymbol(":=", 10, true));
    t->insert(t, "+=", newSymbol("+=", 10, true));
    t->insert(t, "-=", newSymbol("-=", 10, true));
    t->insert(t, "*=", newSymbol("*=", 10, true));
    t->insert(t, "/=", newSymbol("/=", 10, true));
    t->insert(t, "%=", newSymbol("%=", 10, true));

    t->insert(t, "&=", newSymbol("&=", 10, true));
    t->insert(t, "|=", newSymbol("|=", 10, true));
    t->insert(t, "~=", newSymbol("~=", 10, true));
    t->insert(t, "^=", newSymbol("^=", 10, true));

    // t->insert(t, "<<=", newSymbol("", 10, true));
    // t->insert(t, ">>=", newSymbol("", 10, true));

    t->insert(t, "+", newSymbol("+", 50, true));
    t->insert(t, "-", newSymbol("-", 50, true));
    t->insert(t, "*", newSymbol("*", 60, true));
    t->insert(t, "/", newSymbol("/", 60, true));
    t->insert(t, "%", newSymbol("%", 60, true));

    t->insert(t, "&", newSymbol("&", 60, true));
    t->insert(t, "|", newSymbol("|", 60, true));
    t->insert(t, "^", newSymbol("^", 60, true));
    t->insert(t, "~", newSymbol("~", 70, true));

    t->insert(t, "<<", newSymbol("<<", 60, true));
    t->insert(t, ">>", newSymbol(">>", 60, true));

    t->insert(t, "!", newSymbol("!", 70, true));

    t->insert(t, "//", newSymbol("//", 0, true));
    t->insert(t, "++", newSymbol("++", 90, true));
    t->insert(t, "--", newSymbol("--", 90, true));

    t->insert(t, "**", newSymbol("**", 70, true));

    t->insert(t, ":", newSymbol(":", 90, true));
    t->insert(t, ".", newSymbol(".", 90, true));

    t->insert(t, "!=", newSymbol("!=", 80, true));
    t->insert(t, "==", newSymbol("==", 80, true));

    t->insert(t, ">=", newSymbol(">=", 80, true));
    t->insert(t, "<=", newSymbol("<=", 80, true));
    t->insert(t, ">", newSymbol(">", 80, true));
    t->insert(t, "<", newSymbol("<", 80, true));

    t->insert(t, "&&", newSymbol("&&", 70, true));
    t->insert(t, "||", newSymbol("||", 70, true));

    t->insert(t, "import", newSymbol("import", 0, true));
    t->insert(t, "print", newSymbol("print", 0, true));

    return t;
}

Symbol* newSymbol(const char* name, u8 precedence, bool reserved) {
    Symbol* symbol = (Symbol*) pMalloc(sizeof (Symbol));

    symbol->name = name;
    symbol->precedence = precedence;
    symbol->reserved = reserved;

    return symbol;
}

