
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
 * this horrible conditional just checks if the precedence of the operator on top of the stack is
 * less than the precedence of the operator we are holding (thinking about putting on the stack)
 *
 * templating the Stack might help
 *
 * if the operator stack is empty we don't care
 */
static inline bool canPop(Stack<ASTNode>* os, ASTNode* node) {
    return !os->isEmpty() && (os->peek()->token->op->precedence > node->token->op->precedence);
}

/**
 *
 */
static void parseOperation(Stack<ASTNode>* es, Stack<ASTNode>* os, ASTNode* node) {
    ASTNode* lhs = null;
    ASTNode* rhs = null;

    if (node->token->op->unary) {
        ASTNode* temp = os->peek();

        if (!temp->token->op->unary) { // unary operators can only pop and apply other unary operators
            return;
        }

        if (node->token->op->postfix) {
            lhs = es->pop();

        } else {
            rhs = es->pop();
        }
    } else {
        rhs = es->pop();
        lhs = es->pop();
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

    ASTNode* op = NULL;

    u32 i = 0;
    while (i < (Lexer::tokenCount)) {

        switch ((int) tokens[i].tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ')':
                while (os->peek()) {
                    if (((Token*) os->peek())->tt == '(') {
                        break;
                    }

                    parseOperation(es, os, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::add(
                            MS_ERROR,
                            "Missing open parentheses.\n",
                            null,
                            tokens[i].filename,
                            tokens[i].line,
                            tokens[i].column
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
                op = nodify(tokens, i);

                if (op->token->op->call) {
                    // the open paren is being used as the 'grouping' operator
                }

            default:
                op = nodify(tokens, i);

                while (canPop(os, op)) {
                    parseOperation(es, os, os->pop());
                }

                os->push(op);
                break;
        }

        i++;
    }

    while (!os->isEmpty()) {
        parseOperation(es, os, os->pop());
    }

    ASTNode* root = es->pop();

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

