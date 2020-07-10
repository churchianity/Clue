
#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#include "table.h"
#include "token.h"


typedef struct ASTNode {
    Token* token;

    ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;

    bool (*addChild) (ASTNode*, ASTNode*);
    void (*traverse) (ASTNode*, void (*callback) (const ASTNode*));
} ASTNode;

ASTNode* newNode(Token* token, Table* symbolTable);

#endif

