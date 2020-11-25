
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

static void appendInferredTypeInformation(ASTNode* node) {}

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

static OperatorAssociativityEnum associativity(ASTNode* node) {
    switch ((int) node->token->tt) {
        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
        case TT_IMPORT:
            return OA_NONE;

        case ':':
        case ',':
        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
        case '+':
        case '-':
            if ((node->flags & NF_UNARY) != 0) {
                return OA_RIGHT_TO_LEFT;
            }

            return OA_LEFT_TO_RIGHT;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) != 0) {
                return OA_LEFT_TO_RIGHT;
            }
        case '~':
        case '!':
        case TT_EXPONENTIATION:
        case '@':
        case '$':
        case '(':
            return OA_RIGHT_TO_LEFT;

        default:
            die("unknown operator type: %u\n", node->token->tt); return OA_NONE;
    }
}

static u8 precedence(ASTNode* node) {
    switch ((int) node->token->tt) {
        case ',':
            return 0;

        case '=':
        case TT_COLON_EQUALS:
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_MODULO_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
            return 1;

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
            return 2;

        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:
            return 3;

        case '+':
        case '-':
            if ((node->flags & NF_UNARY) != 0) {
                return 6;
            }

            return 4;

        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:
            return 5;

        case '~':
        case '!':
            return 6;

        case TT_EXPONENTIATION:
            return 7;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) != 0) {
                return 6;
            }

        case '(': // @NOTE this may or may not be correct for '{' and '['
        case '{':
        case '[':
            if ((node->flags & NF_PUNCTUATOR) != 0) {
                return 0;
            }
        case '@':
        case '$':
        case '.':
        case ':':
        case TT_IMPORT:
            return 8;

        default:
            die("unknown operator type: %u\n", node->token->tt); return 0;
    }
}

static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    if (os->isEmpty() || ((node->flags & NF_PUNCTUATOR) != 0)) {
        print(node);
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
static Array<ASTNode>* parseExpression(u32 startIndex, u32 endIndex, Array<Token>* tokens) {
    static auto statements = new Array<ASTNode>();
    static auto es = new Array<ASTNode>();
    static auto os = new Array<ASTNode>();

    u32 i = startIndex;

    while (i < endIndex) {
        switch ((int) tokens->data[i]->tt) { // casting because ascii chars are their own token type not defined in TokenTypeEnum

            // skip over import and its operand
            case TT_IMPORT: i += 2; continue;

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
                while (canPopAndApply(os, node)) parseOperation(es, os->pop());

                os->push(node);
            } break;

            // this is the end of some statement or expression
            case ';': {
                while (!os->isEmpty()) {
                    // we shouldn't have any open punctuators at this stage - if we do, there's a mismatch
                    const auto token = os->peek()->token;

                    switch ((int) token->tt) {
                        case '(':
                            Reporter::report(E_MISSING_CLOSE_PAREN, null, token->filename, token->line, token->column);
                            break;

                        case '[':
                            Reporter::report(E_MISSING_CLOSE_BRACKET, null, token->filename, token->line, token->column);
                            break;

                        case '{':
                            Reporter::report(E_MISSING_CLOSE_BRACE, null, token->filename, token->line, token->column);
                            break;
                    }

                    parseOperation(es, os->pop());
                }

                auto expression = es->pop();

                if (!es->isEmpty()) { // if we still have expressions on the stack, they are leftovers
                    Reporter::report(E_LEFTOVER_OPERAND, es->peek());
                }

                statements->push(expression);

                delete es;
                delete os;
                es = new Array<ASTNode>();
                os = new Array<ASTNode>();
            } break;

            // the end of some group of expressions/statements, a function call, or a function definition argument list
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

            // the end of either an array indexer expression or array literal
            case ']': {
                while (os->peek()) {
                    if (os->peek()->token->tt == '[') break;

                    parseOperation(es, os->pop());
                }

                if (os->isEmpty()) {
                    Reporter::report(E_MISSING_OPEN_BRACKET, null, tokens->data[i]->filename, tokens->data[i]->line, tokens->data[i]->column);
                }

                parseOperation(es, os->pop());

            } break;
        }

        i++;
    }

    return statements;
}

/**
 * Given a list of |tokens| return the root node of an abstract syntax tree.
 */
Array<ASTNode>* parse(Array<Token>* tokens) {
    return parseExpression(0, tokens->length, tokens);

    /*
            case TT_IMPORT: {
                program->push(parseExpression(i, i + 2, tokens));
                endOfLastExpressionIndex = i + 1;
            } break;

            case ';': {
                if (endOfLastExpressionIndex == (i - 1)) {
                    const auto token = tokens->data[i];
                    Reporter::add(W_USELESS_SEMICOLON, null, token->filename, token->line, token->column);
                }

                program->push(parseExpression(endOfLastExpressionIndex + 1, i, tokens));
                endOfLastExpressionIndex = i;
            } break;
        }

        i++;
    }

    return program;
    */
}

