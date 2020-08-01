
#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"


extern ASTNode* parse(Token tokens[], u32 tokenCount);

#endif

