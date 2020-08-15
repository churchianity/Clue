
#ifndef NODE_H
#define NODE_H

#include "array.hpp"
#include "table.hpp"
#include "token.h"


/*
struct ASTBNode {
    Token* token;
};

struct ASTBranchNode : ASTBNode {
    ASTBNode* children   = null;
    u32 childrenCount    = 0;
    u32 maxChildrenCount = 0;

    Operator* op;
}

struct ASTBinaryOperatorNode : ASTBranchNode {
    u32 maxChildrenCount = 2;
};

struct ASTUnaryOperatorNode: ASTBranchNode {
    u32 maxChildrenCount = 1;
};
*/

enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT    = -1,
    OA_NONE             =  0,
    OA_RIGHT_TO_LEFT    =  1
};
// @TODO make this more polymorphic
struct ASTNode {
    Token* token = null;

    ASTNode* children    = null;
    u32 childrenCount    = 0;
    u32 maxChildrenCount = 0;

    // everything below here is only relevant information for a relatively short period
    // of this struct's lifecycle .. think about ways to get rid of this
    // deriving them *always* *everytime* is probably too slow, and not worth any aesthetic
    // benefits

    // associativity is looked up in a table, and then modified by the flags below
    // OperatorAssociativityEnum associativity = OA_LEFT_TO_RIGHT;

    // precedence is looked up in a table, and then modified by the flags below
    // u8 precedence = 8;

    // all of these require some degree of peeking backwards in the tokens array
    bool unary      = false; // useful for telling if a '+' is being used as unary plus, for example
    bool postfix    = false; // ++s vs. s++
    bool call       = false; // func(foo, bar, baz); vs. func(foo: Int, bar: String) { ...
    bool punctuator = false; // the above, vs. (2 + 1) * 5
};

extern void traverse(ASTNode* self, void (*callback) (ASTNode*));
extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));

extern OperatorAssociativityEnum associativity(u32 tt, bool unary, bool postfix);
extern u8 precedence(u32 tt, bool unary, bool postfix);
extern ASTNode* nodify(Array<Token>* tokens, u32 i);

extern void addChild(ASTNode* self, ASTNode* child);
extern void freeNode(ASTNode* node);

#endif

