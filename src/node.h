
#ifndef NODE_H
#define NODE_H

#include "array.hpp"
#include "token.h"

enum OperatorAssociativityEnum {
    OA_LEFT_TO_RIGHT    = -1,
    OA_NONE             =  0,
    OA_RIGHT_TO_LEFT    =  1
};

// @TODO make this more polymorphic
// ASTNodes are too general/overloaded.
// they, by far, can take the most forms of any other piece of data in the program
// but over the course of the program lifetime, they can and should be refined down
// right now they aren't at all.
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
    bool call       = false; // func(foo, bar, baz); vs. func(foo: Int, bar: String) { ... }
    bool punctuator = false; // the above, vs. (2 + 1) * 5
};

struct ASTSymbolNode : ASTNode {
    void* closure = null;
};

struct ASTNumberNode : ASTNode {};
struct ASTStringNode : ASTNode {};









struct Program {
    Array<ASTNode>* statements = new Array<ASTNode>(10);
};









extern void traverse(ASTNode* self, void (*callback) (ASTNode*));
extern void traverse(ASTNode* self, void (*callback) (const ASTNode*));

extern OperatorAssociativityEnum associativity(u32 tt, bool unary, bool postfix);
extern u8 precedence(u32 tt, bool unary, bool postfix);
extern ASTNode* nodify(Array<Token>* tokens, u32 i);
extern void addChild(ASTNode* self, ASTNode* child);

#endif

