
#ifndef NODE_H
#define NODE_H

#include "array.hpp"
#include "token.h"
#include "types.h"


enum ASTNodeTypeEnum {
    NT_NONE,
    NT_NUMBER,
    NT_STRING
};

enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT = -1,
    OA_NONE          =  0,
    OA_RIGHT_TO_LEFT =  1
};

enum ASTNodeFlagsEnum {
    NF_UNARY      = 1,
    NF_POSTFIX    = 2,
    NF_CALL       = 4,
    NF_PUNCTUATOR = 8
};

struct Closure {
    const char* name;
};

struct ASTNode {
    Token* token;
    Closure* closure;

    Array<ASTNode>* children;

    ASTNodeTypeEnum type;
    u8 flags;
};

extern void traverse(ASTNode* self, void (*callback) (ASTNode*));

extern ASTNode* nodify(Array<Token>* tokens, u32 i);
extern void freeTree(ASTNode* root);
extern void addChild(ASTNode* self, ASTNode* child);

#endif

