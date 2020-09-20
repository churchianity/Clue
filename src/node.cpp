
#include "array.hpp"
#include "clue.h"
#include "lexer.h"
#include "node.h"
#include "print.h"
#include "reporter.h"
#include "table.hpp"
#include "token.h"


/**
 * Iterates over the tree and calls |callback| on each node, with |root| as an argument.
 * Post-order traversal.
 */
void traverse(ASTNode* node, void (*callback) (ASTNode*)) {
    if (!node) {
        print("root node is null...\n");
        return;
    }

    if (node->children) { // nodes without children have it null'ed
        for (u32 i = 0; i < node->children->length; i++) {
            traverse(node->children->data[i], callback);
        }
    }

    callback(node);
}

OperatorAssociativityEnum associativity(ASTNode* node) {
    switch ((int) node->token->tt) {
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
        case TT_EXPONENTIATION_EQUALS:
        case TT_IMPORT:
            return OA_NONE;

        case ':':
        case ',':
        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
        case '+':
        case '-':
            if ((node->flags & NF_UNARY) != 0) {
                return OA_RIGHT_TO_LEFT;
            }

            return OA_LEFT_TO_RIGHT;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) != 0) {
                return OA_LEFT_TO_RIGHT;
            }
        case '~':
        case '!':
        case TT_EXPONENTIATION:
        case '@':
        case '$':
        case '(':
            return OA_RIGHT_TO_LEFT;

        default:
            die("unknown operator type: %u\n", node->token->tt); return OA_NONE;
    }
}

u8 precedence(ASTNode* node) {
    switch ((int) node->token->tt) {
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
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
            return 2;

        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
            return 3;

        case '+':
        case '-':
            if ((node->flags & NF_UNARY) != 0) {
                return 6;
            }

            return 4;

        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 5;

        case '~':
        case '!':
            return 6;

        case TT_EXPONENTIATION:
            return 7;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) != 0) {
                return 6;
            }

        case '(': // @NOTE this may or may not be correct for '{' and '['
        case '{':
        case '[':
            if ((node->flags & NF_PUNCTUATOR) != 0) {
                return 0;
            }
        case '@':
        case '$':
        case '.':
        case ':':
        case TT_IMPORT:
            return 8;

        default:
            die("unknown operator type: %u\n", node->token->tt); return 0;
    }
}

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        Reporter::report(E_MISSING_OPERAND_FOR_OPERATOR, self);
        return;
    }

    self->children->push(child);
}

/**
 * Resolve a token into a node.
 * This requires some amount of peeking, so the whole Lexer::tokens array should be passed w/ the index of the operator.
 */
ASTNode* nodify(Array<Token>* tokens, u32 i) {
    ASTNode* node = (ASTNode*) pCalloc(sizeof (ASTNode), 1);

    node->token = tokens->data[i];

    switch (node->token->tt) {
        default:
            break;

        case TT_SYMBOL:
        case TT_NUMERIC:
        case TT_STRING:
            node->children = null;
            return node;
    }

    // if it's not an operand itself, it should have some number of operands (children)
    node->children = new Array<ASTNode>();

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) { // is unary prefix, or a opening punctuator
        switch ((int) node->token->tt) {
            case TT_IMPORT: // special case, shouldn't actually do much here
            case '~':
            case '!':
            case '@':
            case '$':
            case '+':
            case '-':
            case TT_INCREMENT:
            case TT_DECREMENT:
                node->flags |= NF_UNARY;
                break;

            case '(':
            case '[':
            case '{':
                node->flags |= NF_PUNCTUATOR;
                break;

            default:
                Reporter::report(E_INVALID_OPERATOR, node);
                break;
        }
    } else if ((tokens->data[i]->tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) { // is a function call
        node->flags |= NF_CALL;

    } else if ((tokens->data[i]->tt == TT_INCREMENT) || (tokens->data[i]->tt == TT_DECREMENT)) { // is postfix unary
        node->flags |= NF_UNARY;
        node->flags |= NF_POSTFIX;

    } else {
        // should be a binary operator... if it isn't, report it.
        if (tokenTypeBinaryness(tokens->data[i]->tt) < 1) {
            Reporter::report(E_EXPECTING_BINARY_OPERATOR, node);
        }
    }

    return node;
}

