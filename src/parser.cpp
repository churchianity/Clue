
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
    return !os->isEmpty() && (node->precedence <= os->peek()->precedence);
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
    } else {

        ASTNode* rhs = es->pop();
        ASTNode* lhs = es->pop();

        if (!(rhs || lhs)) {
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
 */
static ASTNode* shuntingYard(Token tokens[], u32 tokenCount) {
    Stack<ASTNode>* es = new Stack<ASTNode>(10, true);
    Stack<ASTNode>* os = new Stack<ASTNode>(10, true);

    ASTNode* opNode = null; // @TODO remove this

    u32 i = 0;
    while (i < tokenCount) {

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
                        MS_ERROR, "Missing open parentheses.",
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
ASTNode* parse(Token tokens[], u32 tokenCount) {
    return shuntingYard(tokens, tokenCount);
}

