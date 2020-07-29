
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


/**
 * checks if the precedence of the operator on top of the stack is less than the precedence of
 * the operator we are holding (thinking about putting on the stack)
 *
 * if the operator stack is empty we don't care
 */
static inline bool canPop(Stack<ASTNode>* os, ASTNode* node) {
    return !os->isEmpty() && (os->peek()->precedence > node->precedence);
}

/**
 *
 */
static void parseOperation(Stack<ASTNode>* es, Stack<ASTNode>* os, ASTNode* node) {
    ASTNode* lhs = null;
    ASTNode* rhs = null;

    if (node->unary) {
        if (!os->peek()->unary) { // unary operators can only pop and apply other unary operators
            return;
        }

        if (node->postfix) {
            lhs = es->pop();

        } else {
            rhs = es->pop();
        }
    } else {
        rhs = es->pop();
        lhs = es->pop();

        if (!rhs) {
            Reporter::report(
                MS_ERROR, "missing right hand operand for binary operator",
                null, node->token->filename, node->token->line, node->token->column
            );

            exit(1);

        } else if (!lhs) {
            Reporter::report(
                MS_ERROR, "missing left hand operand for binary operator",
                null, node->token->filename, node->token->line, node->token->column
            );

            exit(1);
        }
    }

    addChild(node, lhs);
    addChild(node, rhs);

    es->push(node);
}

/**
 * Parses expressions into an AST.
 */
static ASTNode* shuntingYard(Token tokens[]) {
    Stack<ASTNode>* es = new Stack<ASTNode>(10, true);
    Stack<ASTNode>* os = new Stack<ASTNode>(10, true);

    ASTNode* opNode = null; // @TODO remove this

    u32 i = 0;
    while (i < (Lexer::tokenCount)) {

        switch ((int) tokens[i].tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ')':
                while (os->peek()) {
                    if (os->peek()->token->tt == '(') {
                        break;
                    }

                    parseOperation(es, os, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::add(
                        MS_ERROR, "Missing open parentheses.\n",
                        null, tokens[i].filename, tokens[i].line, tokens[i].column
                    );

                    break;
                }

                os->pop(); // discard opening parens
                break;

            case TT_SYMBOL:
                // @TODO lookup in a symbol table, and act more like a operator if it's a weird operator keyword like 'sizeof'
                // we should also maybe resolve scope at this point, but we haven't yet implemented closures
                es->push(nodify(tokens, i));
                break;

            case TT_STRING:
            case TT_NUMERIC:
                es->push(nodify(tokens, i));
                break;

            case '(':
                opNode = nodify(tokens, i); // @ROBUSTNESS will we always make the right kind of node here?

                if (opNode->call) {
                    // the open paren is being used as the 'grouping' operator
                }

                os->push(opNode);

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
ASTNode* parse(Token tokens[]) {
    return shuntingYard(tokens);
}

