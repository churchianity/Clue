
#ifndef RUNTIME_H
#define RUNTIME_H

#include "node.h"
#include "types.h"
#include "value.h"

extern Value* eval(ASTNode* node);
extern void interactive();
extern void doIt(char* codeBuffer, const char* filename);

#endif

