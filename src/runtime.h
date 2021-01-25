
#ifndef RUNTIME_H
#define RUNTIME_H

#include "array.hpp"
#include "node.h"


namespace Runtime {
    extern void interactive();
    extern void doIt(char* buffer, const char* filename);
    extern void printProgramTree(Array<ASTNode>* program);
}

#endif

