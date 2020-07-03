
#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"


u32 precedence(Token* token);
ASTNode* parse(Token tokens[]);

#endif

