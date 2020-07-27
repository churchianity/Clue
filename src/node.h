
#ifndef NODE_H
#define NODE_H

#include "table.hpp"
#include "token.h"


struct ASTNode {
    Token* token;

    ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;
};

extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));
extern ASTNode* nodify(Token tokens[], u32 currentIndex);
extern u8 precedence(u32 tt, bool unary, bool postfix);
extern void addChild(ASTNode* self, ASTNode* child);

#endif

