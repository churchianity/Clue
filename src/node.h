
#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#include "table.h"
#include "token.h"


typedef struct ASTNode {
    Token* token;

    struct ASTNode* children;
    u32 childrenCount;
    u32 maxChildrenCount;

    bool (*addChild) (struct ASTNode*, struct ASTNode*);
    void (*toString) (const struct ASTNode*);
    void (*traverse) (struct ASTNode*, void (*callback) (struct ASTNode*));
} ASTNode;

ASTNode* newLeafASTNode(Token* token, Table* symbolTable);
ASTNode* newParentASTNode(Token* token, Table* symbolTable);

#endif

