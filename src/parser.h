
#ifndef PARSER_H
#define PARSER_H

#include "array.hpp"
#include "node.h"
#include "token.h"

extern ASTNode* Parser_parse(Array<Token>* tokens);

#endif

