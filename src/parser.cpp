
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

    u32 i = 0;
    while (i < (Lexer::tokenCount)) {

        switch ((int) tokens[i].tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(es, nodify(tokens, i));
                break;

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

            case '(':
            default:
                ASTNode* node = nodify(tokens, i);

                // this horrible conditional just checks if the precedence of the operator on top of the stack is
                // less than the precedence of the operator we are holding (thinking about putting on the stack)
                //
                // templating the Stack might help
                //
                // if the operator stack is empty we don't care
                while (!os->isEmpty(os)
                        && (((ASTNode*) os->peek(os))->token->op->precedence > node->token->op->precedence)) {

                    parseOperation(es, os, (ASTNode*) os->pop(os));
                }

                os->push(os, node);
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

