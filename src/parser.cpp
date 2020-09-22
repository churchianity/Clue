
#include "array.hpp"
#include "clue.h"
#include "node.h"
#include "lexer.h"
#include "reporter.h"
#include "print.h"
#include "table.hpp"
#include "token.h"


// @NOTE |node| is the operator ':', not the node to which we append the type info really.
static void appendTypeInformation(ASTNode* node) {
    node->children->data[0]->type = node->children->data[1]->type;
}

static void appendInferredTypeInformation(ASTNode* node) {
    // @TODO
}

static inline void reportSpecificUnaryOperatorMissingOperand(ASTNode* node) {
    switch ((int) node->token->tt) {
        case '+':
        case '-':
        case '~':
        case '!':
        case '@':
        case '$':

        default:
            Reporter::report(E_MISSING_OPERAND_FOR_UNARY_OPERATOR, node);
            break;
    }
}

static inline void reportSpecificBinaryOperatorMissingOperand(ASTNode* node) {
    switch ((int) node->token->tt) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':

        case '=':
        case TT_COLON_EQUALS:

        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:

        case '&':
        case '|':
        case '^':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:

        case ':':
        case ',':

        default:
            Reporter::report(E_MISSING_OPERAND_FOR_BINARY_OPERATOR, node);
            break;
    }
}

static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    if (os->isEmpty() || ((node->flags & NF_PUNCTUATOR) != 0)) {
        return false;
    }

    const auto top = os->peek();

    if (associativity(node) == OA_LEFT_TO_RIGHT) {
        return precedence(node) <= precedence(top);

    } else if (associativity(node) == OA_RIGHT_TO_LEFT) {
        return precedence(node) < precedence(top);
    }

    // this happens when you have multiple operators between two semicolons (or the beginning of the file and one semicolon) that shouldn't be part of one expression
    // you can't have multiple assignments in one expression ie: x := y := 4;
    Reporter::add(E_NON_ASSOCIATIVE_SUBEXPRESSION, node);
    return false;
}

static void parseOperation(Array<ASTNode>* es, ASTNode* node) {
    if ((node->flags & NF_PUNCTUATOR) != 0) return;

    if (!tokenTypeIsOperator(node->token->tt)) {
        es->push(node); // operands are (sometimes) an operation that return themselves
        return;
    }

    if ((node->flags & NF_UNARY) != 0) {
        ASTNode* child = es->pop();

        if (!child) reportSpecificUnaryOperatorMissingOperand(node);

        addChild(node, child);

    } else { // is binary

        // rhs was pushed most recently, and this matters (2 - 4 vs. 4 - 2).
        ASTNode* rhs = es->pop();
        ASTNode* lhs = es->pop();

        if (!(rhs && lhs)) reportSpecificBinaryOperatorMissingOperand(node);

        addChild(node, lhs);
        addChild(node, rhs);

        // check if this operation is appending type information, in which case we should do so now
        switch ((int) node->token->tt) {
            case ':': appendTypeInformation(node); break;
            case TT_COLON_EQUALS: appendInferredTypeInformation(node); break;
        }
    }

    es->push(node);
}

/**
 * Parses an Array of |tokens| into an AST expression node..
 *
 * You shouldn't call this unless you have a good reason to believe that there is an expression between
 * the tokens array @ |startIndex| and |endIndex|, inclusive of start but not end.
 *
 * this is basically shunting-yard with some bells & whistles to allow function calls, unary operators, postfix/prefix etc.
 */
static ASTNode* parseExpression(u32 startIndex, u32 endIndex, Array<Token>* tokens) {
    auto es = new Array<ASTNode>();
    auto os = new Array<ASTNode>();

    u32 i = startIndex;

    while (i < endIndex) {
        switch ((int) tokens->data[i]->tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum

            case ')': {
                while (os->peek()) {
                    if (os->peek()->token->tt == '(') break;

                    parseOperation(es, os->pop());
                }

                if (os->isEmpty()) { // we never found a matching open paren...
                    Reporter::report(E_MISSING_OPEN_PAREN, null, tokens->data[i]->filename, tokens->data[i]->line, tokens->data[i]->column);
                    break;
                }

                if ((os->peek()->flags & NF_CALL) == 0) {
                    os->pop(); // discard opening parens
                }
            } break;

            // these types only ever act as operands
            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC:
                es->push(nodify(tokens, i));
                break;

            // operators
            default: {
                const auto node = nodify(tokens, i);

                // handle precedence & associativity before pushing the operator onto the stack
                while (canPopAndApply(os, node)) {
                    parseOperation(es, os->pop());
                }

                os->push(node);
            } break;
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
Array<ASTNode>* parse(Array<Token>* tokens) {
    Array<ASTNode>* program = new Array<ASTNode>();

    u32 i = 0;
    u32 lastSemicolonIndex = -1;

    while (i < tokens->length) {
        switch ((int) tokens->data[i]->tt) {
            case ';':
                if (lastSemicolonIndex == (i - 1)) {
                    const auto token = tokens->data[i];
                    Reporter::add(W_USELESS_SEMICOLON, null, token->filename, token->line, token->column);
                }

                program->push(parseExpression(lastSemicolonIndex + 1, i, tokens));
                lastSemicolonIndex = i;
                break;

            case '{':
            case '[':
            case '}':
            case ']':
                break;
        }

        i++;
    }

    return program;
}

