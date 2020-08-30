
#ifndef NODE_H
#define NODE_H

#include "types.h"
#include "array.hpp"
#include "token.h"


enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT    = -1,
    OA_NONE             =  0,
    OA_RIGHT_TO_LEFT    =  1
};

enum ASTNodeFlagsEnum {
    NF_UNARY        = 1,
    NF_POSTFIX      = 2,
    NF_CALL         = 4,
    NF_PUNCTUATOR   = 8
};

struct Closure {
    const char* name;
};

struct ASTNode {
    Token* token = null;
    Closure* closure = null;

    Array<ASTNode>* children = null;

    u8 flags = 0; // @TODO replace with a single u8...
    // bool unary      = false; // useful for telling if a '+' is being used as unary plus, for example
    // bool postfix    = false; // ++s vs. s++
    // bool call       = false; // func(foo, bar, baz); vs. func(foo: Int, bar: String) { ... }
    // bool punctuator = false; // the above, vs. (2 + 1) * 5
};

struct Program {
    Array<ASTNode>* statements = new Array<ASTNode>(10);
};

extern void traverse(ASTNode* self, void (*callback) (ASTNode*));

extern OperatorAssociativityEnum associativity(ASTNode* node);
extern u8 precedence(ASTNode* node);

extern ASTNode* nodify(Array<Token>* tokens, u32 i);
extern void freeTree(ASTNode* root);
extern void addChild(ASTNode* self, ASTNode* child);

#endif

