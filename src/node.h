
#ifndef NODE_H
#define NODE_H

#include "table.hpp"
#include "token.h"

struct ASTNode {
    Token* token;
};

struct ASTOperatorNode : ASTNode {
    Operator* op;

    ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;
};

struct ASTSymbolNode : ASTNode {
    Symbol* symbol;
};

extern void traverse(ASTOperatorNode* self, void (*callback) (const ASTNode*));
extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));

extern ASTOperatorNode* makeOperatorNode(Token tokens[], u32 currentIndex);
extern ASTSymbolNode* makeSymbolNode(Token* token);
extern ASTNode* makeNode(Token* token);

extern u8 precedence(u32 tt, bool unary, bool postfix);
extern void addChild(ASTOperatorNode* self, ASTNode* child);

#endif

