
#ifndef NODE_H
#define NODE_H

#include "array.hpp"
#include "token.h"
#include "types.h"

struct ASTNode {
    Token* token;
    // Closure* closure;

    Array<ASTNode>* children;

    u8 flags;
};


#endif

