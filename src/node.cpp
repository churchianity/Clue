
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
        case '}':
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

        // comparison
        case TT_GREATER_THAN_OR_EQUAL:
        case '>':
        case TT_LESS_THAN_OR_EQUAL:
        case '<':
            return 2;

        // unary & binary plus & minus
        case '+':
        case '-':
            if (unary) {
                return 7;
            }

            return 3;

        // other arithmetic...
        case '*':
        case '/':
        case '%':

        // bitwise binary operations...
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 4;

        // unary bitwise is slightly higher than binary!
        case '~':
            return 5;

        // logical operator (non-comparison)
        case TT_NOT_EQUALS:
        case TT_EQUALITY:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_AND:
            return 6;

        // negation is slightly higher!
        case '!':
            return 7;

        // exponentation
        case TT_EXPONENTIATION:
            return 8;

        // i'm not entirely certain why, but you get problems if ( is lower than assignment
        // when it's invoking a function it's pretty high precedence tho
        case '(':
            if (unary) {
                return 2;
            }
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
 *
 * Some notes on grouping operators/punctuators as in [], {}, ()
 * because they are weird...
 *
 * One way to deal with them is to treat openers as sentinels, and then treat the closers as a unary postfix
 * operator, where their 'single' child is:
 *
 *      )   -   an expression (used for grouping/precdence),
 *              a comma separated list of expressions (function call),
 *              a comma separated list of definitions (function arg definition)
 *
 *      }   -   a comma separated list of tuples (dict literal), a group of statements or expressions (closure)
 *      ]   -   a comma separated list of expressions (array literal), a group of expressions (indexer)
 *
 *      ? more bullshit ?
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
                    MS_ERROR, "missing left hand operand for binary operator",
                    null, node->token->filename, node->token->line, node->token->column
                );
                break;

            case '+':
            case '-':
            case TT_INCREMENT:
            case TT_DECREMENT:
            case '~':
            case '!':
                node->maxChildrenCount = 1;
                node->unary = true;
                break;
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

        } else { // is a binary operator or a postfix-ish punctuator
            switch ((int) tokens[i].tt) {
                case ')':
                case ';':
                    node->punctuator = true;
                    return node;

                default:
                    node->maxChildrenCount = 2;
                    break;
            }
        }
    }

    node->precedence = precedence(node->token->tt
                                , node->unary
                                , node->postfix);

    // @TODO calculate associativity here too

    return node;
}

void freeNode(ASTNode* node) {
    //
}

