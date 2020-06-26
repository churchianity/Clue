
#ifndef RUNTIME_H
#define RUNTIME_H

#include "node.h"

void evalPostfixTokens(Token* tokens);
void evaluateOperator(Token* token);
void interactive();

int main(int argc, char* argv[]);

#endif

