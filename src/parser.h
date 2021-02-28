
#ifndef PARSER_H
#define PARSER_H

#include "array.hpp"
#include "node.h"
#include "token.h"

namespace Parser {
    extern ASTNode* parse(Array<Token>* tokens);
}

#endif

