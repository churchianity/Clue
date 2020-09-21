
#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"


extern Array<ASTNode>* parse(Array<Token>* tokens);

#endif

