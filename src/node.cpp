
#include "clue.h"
#include "lexer.h"
#include "node.h"
#include "print.h"
#include "reporter.h"
#include "table.hpp"
#include "token.h"
#include "trace.h"
#include "util.h"


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    if (self->children != null) {
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
        }
    }

    callback(self);
}

/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* self, void (*callback) (const ASTNode*)) {
    if (!self) {
        print("root node is null...\n");
        return;
    }

    if (self->children != null) {
        for (u32 i = 0; i < self->childrenCount; i++) {
            if (self->children + i) {
                traverse(self->children + i, callback);
            }
        }
    }

    callback(self);
}

/**
 * Once we expect a node to be an operator, and have expectations about it regarding:
 *  - unary-ness
 *  - postfix/prefix-ness
 *
 * We can calculate absolute precedence by looking up a base precedence and then modifying it based on
 * the above criteria.
 */
u8 calculatePrecedence(Token* token, bool unary, bool postfix) {
    const auto entry = OperatorTable->lookup(token->tk, token->length);

    switch (entry->value->type) {
        // unary & binary plus & minus
        case '+':
        case '-':
            if (unary) {
                return 6;
            }

        case TT_DECREMENT:
        case TT_INCREMENT:
            if (postfix) {
                return 8;
            }

        case '(':
        default:
            return entry->value->precedence;
    }
}

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        Reporter::add(
            MS_ERROR, "missing operand for operator",
            null, self->token->filename, self->token->line, self->token->column
        );

        return;
    }

    if (self->childrenCount == self->maxChildrenCount) {
        Reporter::report(
            MS_ERROR, "attempting to add an operand to an operator that is already satisfied",
            null, self->token->filename, self->token->line, self->token->column
        );
    }

    if (!self->children) {
        self->children = (ASTNode*) pCalloc(self->maxChildrenCount, sizeof (ASTNode));
    }

    self->children[self->childrenCount++] = *child;
}

/**
 * Resolve a token into a node.
 *
 * Nodes are mostly distinguished by whether or not they have children.
 *
 * Most of the work here is resolving operator precedence, associativity, and unary/postfix flags.
 * That requires some amount of peeking, so the whole Lexer::tokens array should be passed w/ the index of the operator.
 */
ASTNode* nodify(Token tokens[], u32 i) {
    ASTNode* node = (ASTNode*) pMalloc(sizeof (ASTNode));

    node->token = &tokens[i];

    switch (node->token->tt) {
        default:
            break;

        case TT_SYMBOL:
        case TT_NUMERIC:
        case TT_STRING:
            return node;
    }

    if (i < 1 || isOperator(&tokens[i - 1])) { // is unary prefix, or a punctuator used weirdly
        switch ((int) node->token->tt) {
            case '+':
            case '-':
            case '~':
            case '!':
                node->maxChildrenCount = 1;
                node->unary = true;
                break;

            case ';':
                Reporter::add(
                    MS_WARN, "semicolon with nothing before it has no effect",
                    null, node->token->filename, node->token->line, node->token->column
                );

            case '{':
            case '(':
                node->punctuator = true;
                break;

            default:
                Reporter::report(
                    MS_ERROR, "invalid operator",
                    null, node->token->filename, node->token->line, node->token->column
                );

                break;
        }
    } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
        node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
        node->call = true;

    } else if ((tokens[i].tt == TT_INCREMENT) || (tokens[i].tt == TT_DECREMENT)) { // is postfix unary
        node->maxChildrenCount = 1;
        node->unary = true;
        node->postfix = true;

    } else { // is a binary operator or a postfix-ish punctuator
        switch ((int) tokens[i].tt) {
            case ';':
                node->punctuator = true;
                break;

            default:
                node->maxChildrenCount = 2;
                break;
        }
    }

    // calculate precedence...
    node->precedence = calculatePrecedence(node->token, node->unary, node->postfix);

    // calculate associativity...
    const auto entry = OperatorTable->lookup(node->token->tk, node->token->length);
    node->associativity = entry->value->associativity;

    return node;
}

void freeNode(ASTNode* node) {
    //
}

