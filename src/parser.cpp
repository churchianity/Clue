
#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "reporter.h"
#include "print.h"
#include "stack.h"
#include "symbol.h"
#include "table.h"
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
static inline bool canPop(Stack* os, ASTNode* node) {
    return !os->isEmpty(os) && (((ASTNode*) os->peek(os))->token->op->precedence > node->token->op->precedence);
}

/**
 *
 */
static void parseOperation(Stack* es, Stack* os, ASTNode* node) {
    ASTNode* lhs = null;
    ASTNode* rhs = null;

    if (node->token->op->unary) {
        ASTNode* temp = (ASTNode*) os->peek(os);

        if (!temp->token->op->unary) { // unary operators can only pop and apply other unary operators
            return;
        }

        if (node->token->op->postfix) {
            lhs = (ASTNode*) es->pop(es);

        } else {
            rhs = (ASTNode*) es->pop(es);
        }
    } else {
        rhs = (ASTNode*) es->pop(es);
        lhs = (ASTNode*) es->pop(es);
    }

    addChild(node, lhs);
    addChild(node, rhs);

    es->push(es, node);
}

/**
 * Parses expressions into an AST.
 */
static ASTNode* shuntingYard(Token tokens[]) {
    Stack* es = newStack(10, true);
    Stack* os = newStack(10, true);

    ASTNode* op = NULL;

    u32 i = 0;
    while (i < (Lexer::tokenCount)) {

        switch ((int) tokens[i].tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ')':
                while (os->peek(os)) {
                    if (((Token*) os->peek(os))->tt == '(') {
                        break;
                    }

                    parseOperation(es, os, (ASTNode*) os->pop(os));
                }

                if (os->isEmpty(os)) { // we never found a matching open paren...
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

                os->pop(os); // discard opening parens
                break;

            case TT_SYMBOL:
                // @TODO lookup in a symbol table, and act more like a operator if it's a weird operator keyword like 'sizeof'
                // we should also maybe resolve scope at this point, but we haven't yet implemented closures
                es->push(es, nodify(tokens, i));
                break;

            case TT_STRING:
            case TT_NUMERIC:
                es->push(es, nodify(tokens, i));
                break;

            case '(':
                op = nodify(tokens, i);

                if (op->token->op->call) {
                    // the open paren is being used as the 'grouping' operator
                }

            default:
                op = nodify(tokens, i);

                while (canPop(os, op)) {
                    parseOperation(es, os, (ASTNode*) os->pop(os));
                }

                os->push(os, op);
                break;
        }

        i++;
    }

    while (!os->isEmpty(os)) {
        parseOperation(es, os, (ASTNode*) os->pop(os));
    }

    ASTNode* root = (ASTNode*) es->pop(es);

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

