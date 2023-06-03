
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "array.hpp"
#include "node.h"

ASTNode* shuntingYard(Array<Token>* tokens, u32 startIndex, u32 endIndex);

#endif
