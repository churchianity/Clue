
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

void addChild(ASTNode* self, ASTNode* child) {
    if (!child) {
        Reporter::report(E_MISSING_OPERAND_FOR_OPERATOR, self);
        return;
    }

    self->children->push(child);
}

ASTNode* nodify(Array<Token>* tokens, u32 i) {
    ASTNode* node = (ASTNode*) pCalloc(sizeof (ASTNode), 1);

    node->token = tokens->data[i];

    switch (node->token->tt) {
        default: break;

        case TT_SYMBOL: {
            node->children = null;
            // @NOTE type info is added later.
        } return node;

        case TT_NUMERIC: {
            node->children = null;
            node->type = NT_NUMBER;
        } return node;

        case TT_STRING: {
            node->children = null;
            node->type = NT_STRING;
        } return node;
    }

    // if it's not an operand itself, it should have some number of operands (children)
    node->children = new Array<ASTNode>();

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) { // is unary prefix, or a opening punctuator
        switch ((int) node->token->tt) {
            case '(': // function call (or delimiter)
                node->flags |= NF_PUNCTUATOR;
                break;

            case '[': // array literal
            case '{': // dict literal
                node->flags |= NF_PUNCTUATOR;

            case '~':
            case '!':
            case '@':
            case '$':
            case '+':
            case '-':
            case TT_INCREMENT:
            case TT_DECREMENT:
            case TT_IMPORT: // special case, shouldn't actually do much here
                node->flags |= NF_UNARY;
                break;

            default:
                // @TODO, if the user accidentally puts a space between the last two characters of a multi-character operator, like
                // '+ +'
                // or
                // ': ='
                // or
                // '>> ='
                //
                // then the error message here is not that helpful! we can check what the previous token is and guess what they meant.
                Reporter::report(E_UNEXPECTED_NON_UNARY_OPERATOR, node);
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

