
#ifndef RUNTIME_H
#define RUNTIME_H

#include "array.hpp"
#include "node.h"
#include "table.hpp"
#include "value.h"

extern void Runtime_interactive();
extern void Runtime_doIt(char* buffer, const char* filename);
extern void Runtime_printProgramTree(ASTNode* program);
extern Table<const char, Value>* Runtime_getGlobalSymbolTable();
extern Value Runtime_eval(ASTNode* node);

#endif

