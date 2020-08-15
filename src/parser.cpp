
#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "reporter.h"
#include "print.h"
#include "stack.hpp"
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
static inline bool canPop(Stack<ASTNode>* os, ASTNode* node) {
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

    die("we're checking if we can pop & apply when the assoc : NONE\n"); return false;
}

static void parseOperation(Stack<ASTNode>* es, Stack<ASTNode>* os, ASTNode* node) {
    if (node->punctuator) {
        return;

    } else if (node->unary) {
        if (!(os->isEmpty() || os->peek()->unary)) { // unary operators can only pop and apply other unary operators
            return;
        }

        ASTNode* child = es->pop();

        if (!child) {
            Reporter::report(
                MS_ERROR, "missing operand for unary operator",
                null, node->token->filename, node->token->line, node->token->column
            );
        }

        addChild(node, child);

    } else { // is binary
        ASTNode* rhs = es->pop(); // rhs was pushed most recently
        ASTNode* lhs = es->pop();

        if (!(rhs && lhs)) {
            Reporter::report(
                MS_ERROR, "missing operand for binary operator",
                null, node->token->filename, node->token->line, node->token->column
            );
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
static ASTNode* shuntingYard(Token tokens[], u32 tokenCount) {
    auto es = new Stack<ASTNode>(10, true);
    auto os = new Stack<ASTNode>(10, true);

    ASTNode* opNode = null; // @TODO remove this

    u32 i = 0;
    while (i < tokenCount) {

        switch ((int) tokens[i].tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ')':
                while (os->peek()) {
                    print(os->peek());
                    if (os->peek()->token->tt == '(') {
                        break;
                    }

                    parseOperation(es, os, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::report(
                        MS_ERROR, "Missing open parentheses",
                        null, tokens[i].filename, tokens[i].line, tokens[i].column
                    );

                    break;
                }

                if (!os->peek()->call) {
                    print("hi!\n");
                    os->pop(); // discard opening parens
                }

                break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(nodify(tokens, i));
                break;

            default:
                opNode = nodify(tokens, i);

                while (canPop(os, opNode)) {
                    parseOperation(es, os, os->pop());
                }

                os->push(opNode);
                break;
        }

        i++;
    }

    while (!os->isEmpty()) {

        // we shouldn't have any 'open' punctuators at this stage - if we do, there's a mismatch
        if (os->peek()->token->tt == '(') {
            const auto token = os->peek()->token;

            Reporter::add(
                MS_ERROR, "no matching close paren",
                null, token->filename, token->line, token->column
            );
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
ASTNode* parse(Token tokens[], u32 tokenCount) {
    return shuntingYard(tokens, tokenCount);
}

