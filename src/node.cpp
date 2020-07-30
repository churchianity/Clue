
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
 * There are three pieces of information needed to determine an operator's precedence:
 *     1. Its base type, which overlaps with TokenTypeEnum
 *     2. Whether or not it is being used as a unary operator
 *     3. Whether or not it is being used in postfix
 */
u8 precedence(u32 tt, bool unary, bool postfix) {
    switch (tt) {
        case ')':
        case ']':
        case '{':
        case '}': //????
        case ';':
        case ',':
            return 0;

        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_BITWISE_NOT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_GREATER_THAN_OR_EQUAL:
        case '>':
        case TT_LESS_THAN_OR_EQUAL:
        case '<':
            return 2;

        case '+':
        case '-':
            if (unary) {
                return 7;
            }

            return 3;

        case '*':
        case '/':
        case '%':

        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 4;
        case '~':
            return 5;

        case TT_NOT_EQUALS:
        case TT_EQUALITY:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_AND:
            return 6;

        case '!':
            return 7;

        case TT_EXPONENTIATION:
            return 8;

        case '(':
        case '[':
        case TT_DECREMENT:
        case TT_INCREMENT:
        case '.':
        case ':':
            return 9;

        case TT_SYMBOL:
            // @FIXME do a lookup for global symbols like 'sizeof' equivalent or stuff like that
            die("symbol operator!\n");
            break;
    }

    die("attempt to lookup precedence for unknown operator/tokentype: %c/%d\n", tt, tt);
    return 0; // @NOTE will never get here, die() should always call exit(1) but gcc can't figure that out
}

void addChild(ASTNode* self, ASTNode* child) {
    if (self->childrenCount == self->maxChildrenCount) {
        Reporter::report(
            MS_ERROR, "attempting to add an operand to an operator that is already satisfied",
            null, self->token->filename, self->token->line, self->token->column
        );
    }

    if (!self->children) {
        self->children = (ASTNode*) pMalloc(sizeof (ASTNode) * self->maxChildrenCount);
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

    // check if it's the first token, so we know if we can safely look backwards later
    if (i < 1) {
        // if we pass a non-unary operator as the first token, it's hard to detect because
        // what makes an operator unary in the general case is that it is preceeded by another operator
        //
        // this is a terrible solution - i'm going to forget to add tokentypes when we modify or add
        // new unary operators to the languages @FIXME
        switch ((int) node->token->tt) {
            default:
                Reporter::report(
                    MS_ERROR, "expecting a unary operator here",
                    null, node->token->filename, node->token->line, node->token->column
                );
            case '+': case '-': case '!': case '~': break;
        }
    } else {
        if (isOperator(&tokens[i - 1])) { // is unary prefix
            node->maxChildrenCount = 1;
            node->unary = true;

        } else if ((tokens[i].tt == TT_INCREMENT) || (tokens[i].tt == TT_DECREMENT)) { // is postfix unary
            node->maxChildrenCount = 1;
            node->unary = true;
            node->postfix = true;

        } else if ((tokens[i].tt == '(') && (tokens[i - 1].tt == TT_SYMBOL)) { // is a function call
            node->maxChildrenCount = CLUE_MAX_ARGUMENT_LIST_SIZE;
            node->call = true;

        } else { // is a binary operator
            node->maxChildrenCount = 2;
        }
    }

    node->precedence = precedence(node->token->tt
                                , node->unary
                                , node->postfix);

    // @TODO calculate associativity here too

    return node;
}

