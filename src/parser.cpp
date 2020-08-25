
#include "array.hpp"
#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "reporter.h"
#include "print.h"
#include "symbol.h"
#include "table.hpp"
#include "token.h"
#include "util.h"


// @TODO FUNCTION:
//      definitions
//      invocations
//      array definitions
//      substitution of arrays and comma separated lists
//
//
//              func(a: Int, b: Int, c: Int): void {}
//              foo(players: Player[]): void {}
//
//      ht := getHashTable();
//      v := vec3(2, 1, 3)
//
//      1. func(v.x, v.y, v.z);
//      2. func(v)
//
//      key: string = player name, value: int = player age
//
//      foo(ht);
//
//

/**
 * checks if the precedence of the operator on top of the stack is less than the precedence of
 * the operator we are holding (thinking about putting on the stack)
 *
 * if the operator stack is empty we don't care
 */
static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    if (os->isEmpty() || os->peek()->punctuator) {
        return false;
    }

    const auto top = os->peek();

    if (associativity(node->token->tt, node->unary, node->postfix) == OA_LEFT_TO_RIGHT) {
        return precedence(node->token->tt, node->unary, node->postfix)
            <= precedence(top->token->tt, top->unary, top->postfix);

    } else if (associativity(node->token->tt, node->unary, node->postfix) == OA_RIGHT_TO_LEFT) {
        return precedence(node->token->tt, node->unary, node->postfix)
             < precedence(top->token->tt, top->unary, top->postfix);
    }

    return false;
    // die("we're checking if we can pop & apply when the assoc : NONE\n"); return false;
}

static void parseOperation(Array<ASTNode>* es, Array<ASTNode>* os, ASTNode* node) {
    if (node->punctuator) {
        return;

    } else if (node->unary) {
        ASTNode* child = es->pop();

        if (!child) {
            Reporter::report(E_MISSING_OPERAND_FOR_UNARY_OPERATOR, node); // @REPORT
        }

        addChild(node, child);

    } else { // is binary
        ASTNode* rhs = es->pop(); // rhs was pushed most recently
        ASTNode* lhs = es->pop();

        if (!(rhs && lhs)) {
            Reporter::report(E_MISSING_OPERAND_FOR_BINARY_OPERATOR, node); // @REPORT
        }

        addChild(node, lhs);
        addChild(node, rhs);
    }

    es->push(node);
}

/**
 * Parses expressions into an AST.
 * @TODO re-purpose this into basically a arithmetic/math expression parser,
 * then make subroutines for weird parses
 * like function calls, indexers, etc.
 */
static ASTNode* shuntingYard(Array<Token>* tokens) {
    auto es = new Array<ASTNode>(10);
    auto os = new Array<ASTNode>(10);

    u32 i = 0;
    while (i < tokens->size()) {

        switch ((int) tokens->data[i]->tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ';':



                break;

            case ')':
                while (os->peek()) {
                    if (os->peek()->token->tt == '(') {
                        break;
                    }

                    parseOperation(es, os, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::report(E_MISSING_OPEN_PAREN, null, tokens->data[i]->filename, tokens->data[i]->line, tokens->data[i]->column);
                    break;
                }

                if (!os->peek()->call) {
                    os->pop(); // discard opening parens
                }

                break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(nodify(tokens, i));
                break;

            default:
                const auto node = nodify(tokens, i);

                // handle precedence & associativity before pushing the operator onto the stack
                while (canPopAndApply(os, node)) {
                    parseOperation(es, os, os->pop());
                }

                os->push(node);
                break;
        }

        i++;
    }

    while (!os->isEmpty()) {

        // we shouldn't have any 'open' punctuators at this stage - if we do, there's a mismatch
        if (os->peek()->token->tt == '(') {
            const auto token = os->peek()->token;

            Reporter::report(E_MISSING_CLOSE_PAREN, null, token->filename, token->line, token->column);
        }

        parseOperation(es, os, os->pop());
    }

    ASTNode* root = (ASTNode*) es->pop();

    free(es);
    free(os);

    return root;
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
ASTNode* parse(Array<Token>* tokens) {
    return shuntingYard(tokens);
}

