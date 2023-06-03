
#include "array.hpp"
#include "node.h"
#include "parser.h"
#include "expression.h"
#include "reporter.h"
#include "print.h"
#include "message.h"
#include "runtime.h"
#include "token.h"


ASTNode* Parser_parse(Array<Token>* tokens) {
    ASTNode* programRoot = (ASTNode*) pCalloc(1, sizeof (ASTNode));
    programRoot->children = new Array<ASTNode>();

    auto scope = (Scope*) pMalloc(sizeof (Scope));
    scope->name = "__global";
    scope->parent = null;
    scope->table = Runtime_getGlobalSymbolTable();

    u32 lastExpressionParseIndex = 0;
    u32 i = 0;
    while (i < tokens->length) {
        const auto token = tokens->data[i];
        s32 tt = (s32) token->tt;

        switch(tt) {
            case ';': {
                const auto node = shuntingYard(tokens, lastExpressionParseIndex, i);
                if (node != null) {
                    programRoot->children->push(node);
                }
                lastExpressionParseIndex = ++i;
                continue;
            } break;
        }

        i++;
    }

    return programRoot;
}

