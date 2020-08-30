
#include "array.hpp"
#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "reporter.h"
#include "print.h"
#include "table.hpp"
#include "token.h"


/**
 * Checks if the precedence of the operator on top of the stack is less than
 * the precedence of the operator we are holding (thinking about putting on the stack)
 */
static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    if (os->isEmpty()) { // if the operator stack is empty we don't care
        return false;
    }

    const auto top = os->peek();

    if (associativity(node) == OA_LEFT_TO_RIGHT) {
        return precedence(node) <= precedence(top);

    } else if (associativity(node) == OA_RIGHT_TO_LEFT) {
        return precedence(node) < precedence(top);
    }

    return false;
}

static void parseOperation(Array<ASTNode>* es, ASTNode* node) {
    if (node->flags | NF_UNARY) {
        ASTNode* child = es->pop();

        if (!child) {
            Reporter::report(E_MISSING_OPERAND_FOR_UNARY_OPERATOR, node);
        }

        addChild(node, child);

    } else { // is binary

        // rhs was pushed most recently, and this matters at runtime (2 - 4 vs. 4 - 2).
        ASTNode* rhs = es->pop();
        ASTNode* lhs = es->pop();

        if (!(rhs && lhs)) {
            Reporter::report(E_MISSING_OPERAND_FOR_BINARY_OPERATOR, node);
        }

        addChild(node, lhs);
        addChild(node, rhs);
    }

    es->push(node);
}

/**
 * Parses an Array of |tokens| into an AST expression node..
 *
 * You shouldn't call this unless you have a good reason to believe that there is an expression between
 * the tokens array @ |startIndex| and |endIndex|, inclusive of start but not end.
 *
 * @NOTE this is basically shunting-yard with some bells & whistles to allow function calls, unary operators, postfix/prefix etc.
 */
static ASTNode* parseExpression(u32 startIndex, u32 endIndex, Array<Token>* tokens) {
    auto es = new Array<ASTNode>(10);
    auto os = new Array<ASTNode>(10);

    u32 i = startIndex;

    while (i < endIndex) {
        switch ((int) tokens->data[i]->tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum
            case ')':
                while (os->peek()) {
                    if (os->peek()->token->tt == '(') {
                        break;
                    }

                    parseOperation(es, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::report(E_MISSING_OPEN_PAREN, null, tokens->data[i]->filename, tokens->data[i]->line, tokens->data[i]->column);
                    break;
                }

                if (!(os->peek()->flags | NF_CALL)) {
                    os->pop(); // discard opening parens
                }

                break;

            // these types only ever act as operands
            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(nodify(tokens, i));
                break;

            // all 'standard' operators
            default:
                auto node = nodify(tokens, i);

                // handle precedence & associativity before pushing the operator onto the stack
                while (canPopAndApply(os, node)) {
                    print(node);
                    parseOperation(es, os->pop());
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

        parseOperation(es, os->pop());
    }

    ASTNode* expression = (ASTNode*) es->pop();

    if (!es->isEmpty()) { // if we still have expressions on the stack, they are leftovers
        const auto node = es->peek();
        Reporter::report(E_LEFTOVER_OPERAND, node);
    }

    delete es;
    delete os;

    return expression;
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
Program* parse(Array<Token>* tokens) {
    Program* program = (Program*) pMalloc(sizeof (Program));
    program->statements = new Array<ASTNode>(10);

    u32 i = 0;
    u32 lastExpressionIndex = 0;

    while (i < tokens->length) {
        switch ((int) tokens->data[i]->tt) {
            case ';':
                if (lastExpressionIndex == i) {
                    const auto token = tokens->data[i];
                    Reporter::add(W_USELESS_SEMICOLON, null, token->filename, token->line, token->column);
                }

                program->statements->push(parseExpression(lastExpressionIndex, i, tokens));
                lastExpressionIndex = ++i; // increment past the semicolon
                break;
        }

        i++;
    }

    return program;
}

