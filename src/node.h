

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

/*
enum ASTNodeOperatorTypeEnum {

};
*/

enum ASTNodeFlagsEnum {
    NF_UNARY        = 1,
    NF_POSTFIX      = 2,
    NF_PUNCTUATOR   = 4,
    NF_CALL         = 8,
    NF_INDEXER      = 16,
};

struct ASTNode {
    Token* token;

    Table<const char, Value>* table;
    ASTNode* parent;

    Array<ASTNode>* children;

    u8 flags;
};

void prettyPrintTree(ASTNode* node, const char* indent, bool last);
void traverse(ASTNode* node, void (*callback) (ASTNode*));
void print(ASTNode* node);

#endif

