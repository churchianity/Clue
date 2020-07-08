
#ifndef PRINT_H
#define PRINT_H

#include "token.h"
#include "node.h"
#include "stack.h"
#include "table.h"


void print(const Token* token);
void print(const ASTNode* node);
void print(const Stack* stack);
void print(const Table* table);

void printTokens(const Token tokens[]);

#endif
