

#ifndef NODE_H
#define NODE_H

#include "array.hpp"
#include "print.h"
#include "table.hpp"
#include "token.h"
#include "types.h"
#include "value.h"


enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT = -1,
    OA_NONE          = 0,
    OA_RIGHT_TO_LEFT = 1
};

// usually, the token type is enough to figure out what the operator is trying to do.
// in the other cases, we can set a flag
enum ASTNodeFlagsEnum {
    NF_UNARY          = 1 << 0, //                      ie: -4 vs. 2-4
    NF_POSTFIX        = 1 << 1, //                      ie: count++ vs. ++count
    NF_CALL           = 1 << 2, // function call        ie: func(bar);
    NF_INDEXER        = 1 << 3, // indexer expression   ie: arr[2];
    NF_STRUCT_LITERAL = 1 << 4, // a struct literal     ie: { x := 4, y := 2 }
};

struct ASTNode {
    Token* token;
    Array<ASTNode>* children;
    u8 flags;
};

void prettyPrintTree(ASTNode* node, const char* indent = "", bool last = true);
void traverse(ASTNode* node, void (*callback) (ASTNode*));
void print(ASTNode* node);

#endif

