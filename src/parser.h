
#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "node.h"

Node* parse(Token* tokens);
Node* findRoot(Token* tokens);

#endif

