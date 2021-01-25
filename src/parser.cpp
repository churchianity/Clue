
#include <stdio.h> // remove me

#include "array.hpp"
#include "node.h"
#include "parser.h"
#include "print.h"
#include "token.h"


static inline void reportSpecificUnaryOperatorMissingOperand(ASTNode* node) {
    switch ((int) node->token->tt) {
        case '~':
        case '!':
        case '@':
        case '#':
        case '$':

        case '+':
        case '-':

        default:
            die("missing operand for unary operator: %d\n", node->token->tt);
            break;
    }
}

static inline void reportSpecificBinaryOperatorMissingOperand(ASTNode* node) {
    switch ((int) node->token->tt) {
        case '[': // indexer without a indexee
        case '(': // ???

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':

        case '&':
        case '|':
        case '^':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:

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

        case ':':
        case ',':

        default:
            die("missing operand for binary operator: %d\n", node->token->tt);
            break;
    }
}

static inline OperatorAssociativityEnum associativity(ASTNode* node) {
    switch ((int) node->token->tt) {
        case ':':
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
        case TT_LOGICAL_XOR_EQUALS:
            return OA_RIGHT_TO_LEFT;

        case '+':
        case '-':
            if ((node->flags & NF_UNARY) == NF_UNARY) {
                return OA_RIGHT_TO_LEFT;
            }
        case '*':
        case '/':
        case '%':

        case '&':
        case '|':
        case '^':
        case TT_LEFT_SHIFT:
        case TT_RIGHT_SHIFT:

        case '>':
        case '<':
        case TT_GREATER_THAN_OR_EQUAL:
        case TT_LESS_THAN_OR_EQUAL:

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:
            return OA_LEFT_TO_RIGHT;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) == NF_POSTFIX) {
                return OA_LEFT_TO_RIGHT;
            }
        case '~':
        case '!':
        case TT_EXPONENTIATION:
        case '@':
        case '$':
        case '(': // function invocation
        case '[': // indexer
        case ',':
            return OA_RIGHT_TO_LEFT;

        default:
            die("trying to get associativity of unknown operator type: %u\n", node->token->tt);
            return OA_NONE;
    }
}

static inline u8 precedence(ASTNode* node) {
    if (associativity(node) == OA_NONE) {
        return 0;
        // operators with no associativity implicitly have no precedence because they should never be in a situation where precedence is relevant
    }

    switch ((int) node->token->tt) {
        case ',':

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
        case TT_LOGICAL_XOR_EQUALS:
            return 1;

        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:
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
            // I have no clue why, but changing this to 4 from 7 fixes a precedence bug with exponentiation
            // @TODO figure this out
            return 4;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) == NF_POSTFIX) {
                return 6;
            }

        case '(':
        case '[':
            if ((node->flags & NF_PUNCTUATOR) == NF_PUNCTUATOR) {
                return 0;
            }
        case '@':
        case '#':
        case '$':
        case '.':
        case ':':
        case TT_IMPORT:
            return 8;

        default:
            die("trying to get precedence of unknown operator type: %u\n", node->token->tt);
            return 0;
    }
}

static inline ASTNode* resolveOperatorNode(Array<Token>* tokens, u32 i) {
    ASTNode* node = (ASTNode*) pCalloc(sizeof (ASTNode));

    node->token = tokens->data[i];

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) {
        switch ((int) node->token->tt) {
            default:
                die("unexpected operator that we thought would be unary or a punctuator: %d\n", tokens->data[i]->tt);
                break;

            case ')':
                // empty group or function call w/ no args
                if (tokens->data[i - 1]->tt == ')') break;
            case ']':
                // empty array literal or indexer
                if (tokens->data[i - 1]->tt == '[') break;
            case '}':
                // empty dict literal or code block
                if (tokens->data[i - 1]->tt == '{') break;

            case '(':
                // being here means there's an operator before us, which means this can't be a function call
                node->flags |= NF_PUNCTUATOR;
                break;
            case '[':
                // I guess it's possible for in an operator overloaded case, something like
                // [ 0, 2 ] - [ 1, 3 ] to be valid, but otherwise?
                // this is a special kind of error if it's at the beginning of the file, like
                // you just have a file with line 1:
                // [ 1, 2, 3 ];
                // most of the time this will be assignment of an array literal into some variable, ie:
                // x := [ y, z ];
                node->flags |= NF_UNARY;
                break;

            case '{':
                // if ( ... ) { ... }
                //          ^
                //
                // if the thing before us is an operator, it basically has to be...
                // TT_DO, like:
                //
                // do { ...
                //
                // or, it could be the end of the last expression and the start of a new anonymous namespace, like:
                // thing;
                //      ` last token is a semicolon
                // { ... }
                //
                // in all cases I don't think we care or need to do anything special?
                // not until we hit '}' anyway
                node->flags |= NF_PUNCTUATOR;
                break;

            case '~':
            case '!':
            case '@':
            case '#':
            case '$':
            case TT_INCREMENT:
            case TT_DECREMENT:
            case TT_IMPORT:

            case '+':
            case '-':
                node->flags |= NF_UNARY;
                break;
        }
    } else if ((tokens->data[i]->tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_CALL;

    } else if ((tokens->data[i]->tt == '[') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_INDEXER;

    } else if ((tokens->data[i]->tt == TT_INCREMENT) || (tokens->data[i]->tt == TT_DECREMENT)) {
        node->flags |= NF_UNARY;
        node->flags |= NF_POSTFIX;

    } else {
        if (tokenTypeBinaryness(tokens->data[i]->tt) < 1) {
            die("expecting binary operator, but got op: %d", tokens->data[i]->tt);
        }

        // certain punctuators can trick the parser into thinking that they are binary if the
        // prev token is not an operator or a symbol, check for that here
        if (tokens->data[i]->tt == '(' || tokens->data[i]->tt == '[') {
            const auto prev = tokens->data[i - 1];

            if (prev->tt == TT_NUMERIC || prev->tt == TT_STRING) {
                die("you probably meant to put an operator before this boi\n");
            }
        }
    }

    return node;
}

static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    // @TODO, is it possible to not have to check for punctuators here?
    if (os->isEmpty() || ((node->flags & NF_PUNCTUATOR) != 0)) return false;

    const auto top = os->peek();

    if (associativity(node) == OA_LEFT_TO_RIGHT) {
        return precedence(node) <= precedence(top);

    } else if (associativity(node) == OA_RIGHT_TO_LEFT) {
        return precedence(node) < precedence(top);
    }

    // this happens when you have multiple operators between two semicolons
    // that shouldn't be part of one statement or expression
    // you can't have multiple assignments in one statement ie: x := y := 4;
    // (unless you do x, y := 4, 4;)
    // @NOTE maybe not, this is a bad comment
    // die("non associative sub-expression\n");
    return false;
}

void parseOperation(Array<ASTNode>* es, Array<ASTNode>* os) {
    const auto node = os->pop();

    if ((node->flags & NF_CALL) != 0) {
        print("should be parsing a function call operation LOOOOOL\n");
        return;

    } else if ((node->flags & NF_UNARY) != 0) {
        ASTNode* child = es->pop();

        if (!child) reportSpecificUnaryOperatorMissingOperand(node);

        node->children = new Array<ASTNode>(1);
        node->children->push(child);

    } else {
        // assumed to be a binary operation.
        ASTNode* rhs = es->pop();
        ASTNode* lhs = es->pop();

        if (!(rhs && lhs)) reportSpecificBinaryOperatorMissingOperand(node);

        node->children = new Array<ASTNode>(2);
        node->children->push(lhs);
        node->children->push(rhs);
    }

    es->push(node);
}

static ASTNode* produceExpressionOrStatement(Array<ASTNode>** es, Array<ASTNode>** os) {
    if ((*es)->isEmpty()) {
        die("empty expression, extra semicolon\n");
        return null;
    }

    const auto expressionStatement = (*es)->pop();

    if (!(*es)->isEmpty()) {
        die("leftover operands");
        return null;
    }

    delete *es;
    delete *os;
    const auto _es = new Array<ASTNode>();
    const auto _os = new Array<ASTNode>();
    *es = _es;
    *os = _os;

    return expressionStatement;
}

Array<ASTNode>* Parser :: parse(Array<Token>* tokens) {
    static auto program = new Array<ASTNode>();

    static auto es = new Array<ASTNode>();
    static auto os = new Array<ASTNode>();

    u32 i = 0;
    while (i < tokens->length) {
        switch ((int) tokens->data[i]->tt) {
            case TT_IMPORT: // @TODO
                continue;

            case ';': {
                while (!os->isEmpty()) {
                    const auto token = os->peek()->token;

                    switch ((int) token->tt) {
                        case '(':
                            die("missing closing paren\n");
                            break;
                        case '[':
                            die("missing closing bracket\n");
                            break;
                        case '{':
                            die("missing closing brace\n");
                            break;
                    }

                    parseOperation(es, os);
                }

                const auto node = produceExpressionOrStatement(&es, &os);
                if (node != null) {
                    program->push(node);
                }
            } break;

            case ')': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '(') {
                        break;
                    }

                    parseOperation(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open parens\n");
                    break;
                }

                if ((os->peek()->flags & NF_PUNCTUATOR) != 0) {
                    os->pop(); // discard open parens if it's just used to group

                } else {
                    parseOperation(es, os);
                }
            } break;

            case ']': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '[') break;

                    parseOperation(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open bracket\n");
                    break;
                }

                parseOperation(es, os);

            } break;

            case '}': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '{') break;

                    parseOperation(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open brace\n");
                    break;
                }
            } break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC: {
                const auto node = (ASTNode*) pCalloc(sizeof (ASTNode));
                node->token = tokens->data[i];
                es->push(node);
            } break;

            case '(':
            case '[':
            case '{':
            default: {
                const auto node = resolveOperatorNode(tokens, i);

                while (canPopAndApply(os, node)) parseOperation(es, os);

                os->push(node);
            } break;
        }

        i++;
    }

    return program;
}

