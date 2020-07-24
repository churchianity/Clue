
#ifndef PRINT_H
#define PRINT_H

#include "message.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "stack.h"

extern void print(const Token* token);
extern void print(const ASTNode* node);
extern void print(const Stack* stack);
extern void print(const Table* table);
extern void print(const Message* message);

#endif

