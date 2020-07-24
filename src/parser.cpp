
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
 * Given
 */
static void parseOperation(Stack* es, Stack* os, ASTNode* node) {
    ASTNode* lhs = NULL;
    ASTNode* rhs = NULL;

    if (node->token->op->isUnary) {
        ASTNode* temp = (ASTNode*) os->peek(os);

        if (!temp->token->op->isUnary) { // unary operators can only pop and apply other unary operators
            return;
        }

        if (node->token->op->isPostfix) {
            lhs = (ASTNode*) es->pop(es);

        } else {
            rhs = (ASTNode*) es->pop(es);
        }
    } else {
        rhs = (ASTNode*) es->pop(es);
        lhs = (ASTNode*) es->pop(es);
    }

    ASTNode* t = (ASTNode*) os->pop(os);

    addChild(t, lhs);
    addChild(t, rhs);

    es->push(es, t);
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
                es->push(es, newNode(tokens, i));
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
                            NULL,
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
                ASTNode* op = newNode(tokens, i);

                // while (!os->isEmpty(os) && (((ASTNode*) os->peek(os))->precedence > precedence(tokens[i].tk))) {
                //     parseOperation(es, os, op);
                // }

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

