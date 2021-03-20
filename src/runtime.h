
#ifndef RUNTIME_H
#define RUNTIME_H

#include "array.hpp"
#include "node.h"
#include "table.hpp"
#include "value.h"


namespace Runtime {
    extern void interactive();
    extern void doIt(char* buffer, const char* filename);
    extern void printProgramTree(ASTNode* program);
    extern Table<const char, Value>* getGlobalSymbolTable();
    extern Value eval(ASTNode* node);
}

#endif

