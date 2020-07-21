
#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#include "table.h"
#include "token.h"


typedef struct ASTNode {
    Token* token;

    bool unary : 1;
    bool postfix : 1;
    bool call : 1;
    u8 precedence;

    ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;

} ASTNode;

u32 precedence(Token* token);
void traverse(ASTNode* self, void (*callback) (const ASTNode*));
ASTNode* newNode(Token tokens[], u32 currentIndex);
void addChild(ASTNode* self, ASTNode* child);

#endif

