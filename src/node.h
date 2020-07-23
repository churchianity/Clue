
#ifndef NODE_H
#define NODE_H

#include "table.h"
#include "token.h"


struct ASTNode {
    Token* token;

    bool unary : 1;
    bool postfix : 1;
    bool call : 1;
    u8 precedence;

    ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;

};

extern u32 precedence(Token* token);
extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));
extern ASTNode* newNode(Token tokens[], u32 currentIndex);
extern void addChild(ASTNode* self, ASTNode* child);

#endif

