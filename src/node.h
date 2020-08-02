
#ifndef NODE_H
#define NODE_H

#include "table.hpp"
#include "token.h"


enum OperatorAssociativityEnum {
    OA_LEFT = -1,
    OA_RIGHT = 1
};

struct ASTNode {
    Token* token;

    ASTNode* children    = null;
    u32 childrenCount    = 0;
    u32 maxChildrenCount = 0;

    OperatorAssociativityEnum associativity = OA_LEFT;

    u8 precedence = 0;

    bool unary      = false;
    bool postfix    = false;
    bool call       = false;
    bool punctuator = false;
};

extern void traverse(ASTNode* self, void (*callback) (ASTNode*));
extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));

extern ASTNode* nodify(Token tokens[], u32 currentIndex);

extern u8 precedence(u32 tt, bool unary, bool postfix);
extern void addChild(ASTNode* self, ASTNode* child);
extern void freeNode(ASTNode* node);

#endif

