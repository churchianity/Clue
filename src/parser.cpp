

#include "array.hpp"
#include "node.h"
#include "parser.h"
#include "print.h"
#include "token.h"


static inline void reportSpecificUnaryOperatorMissingOperand(ASTNode* node) {
    switch ((s32) node->token->tt) {
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
    switch ((s32) node->token->tt) {
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
    switch ((s32) node->token->tt) {
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

        // idk section
        case TT_DO:
        case TT_IF:

        case '{':
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

    switch ((s32) node->token->tt) {
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
        case TT_AND:
        case TT_OR:
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
        case TT_NOT:
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
        case '{':
            if ((node->flags & NF_PUNCTUATOR) == NF_PUNCTUATOR) {
                return 0;
            }
        case '@':
        case '#':
        case '$':
        case '.':
        case ':':
        case TT_IMPORT:
        case TT_DO:
        case TT_IF:
            return 8;

        default:
            die("trying to get precedence of unknown operator type: %u\n", node->token->tt);
            return 0;
    }
}

static inline ASTNode* resolveOperatorOrPunctuatorNode(Array<Token>* tokens, u32 i) {
    ASTNode* node = (ASTNode*) pCalloc(sizeof (ASTNode));

    node->token = tokens->data[i];

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) {
        // we think it's a unary operator, but we could be wrong. check.
        switch ((s32) node->token->tt) {
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
                // whether this is a dictionary literal or a code block, it can have a variable number of children.
                node->children = new Array<ASTNode>();
                node->flags |= NF_PUNCTUATOR;
                break;

            case '~':
            case '!':
            case '@':
            case '#':
            case '$':
            case TT_INCREMENT: // @TODO remove or check for compiler flags if we allow increment/decrement.
            case TT_DECREMENT:

            case TT_IMPORT:
            case TT_IF:
            case TT_ELSE:
            case TT_DO:
            case TT_WHILE:

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
        // certain punctuators can trick the parser into thinking that they are binary if the
        // prev token is not an operator or a symbol, check for that here
        // ie:
        //
        //      "string"[0];
        //      4(foo);
        //
        const auto prev = tokens->data[i - 1];
        switch ((s32) tokens->data[i]->tt) {
            default:
                // check if it's actually a binary operator, or a mistake.
                // exceptions can be put above.
                if (tokenTypeBinaryness(tokens->data[i]->tt) < 1) {
                    die("expecting binary operator, but got op: %d", tokens->data[i]->tt);
                }
                break;

            case '(':
            case '[':
                if (prev->tt == TT_NUMERIC || prev->tt == TT_STRING) {
                    die("you probably meant to put an operator before this boi\n");
                }
                break;

            case '{':
                // code block with a expression prior, which means this is either a:
                // if/else if/else/while/for block
                // or a function block
                node->flags |= NF_PUNCTUATOR;
                node->children = new Array<ASTNode>();
                break;
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

void parseOperationIntoExpression(Array<ASTNode>* es, Array<ASTNode>* os) {
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

static ASTNode* shuntingYard(Array<Token>* tokens) {
    auto es = new Array<ASTNode>();
    auto os = new Array<ASTNode>();

    Closure* closure = (Closure*) pMalloc(sizeof (Closure));
    closure->name = "global";
    closure->parent = null;
    closure->table = new Table<const char, Value>();

    ASTNode* oldParent = null;

    ASTNode* currentParent = (ASTNode*) pCalloc(sizeof (ASTNode));
    currentParent->closure = closure;
    currentParent->children = new Array<ASTNode>();

    ASTNode* program = currentParent;

    u32 i = 0;
    while (i < tokens->length) {
        s32 tt = (s32) tokens->data[i]->tt;
        switch (tt) {
            case ';': {
                while (!os->isEmpty()) {
                    s32 tt = (s32) os->peek()->token->tt;

                    if (tt == '(') {
                        // @REPORT missing close paren
                        die("missing close paren");
                        break;

                    } else if (tt == '[') {
                        // @REPORT missing close bracket
                        die("missing close bracket");
                        break;

                    } else if (tt == '{') {
                        // if it's the opener of a code block, then that's fine.
                        if ((os->peek()->flags & NF_PUNCTUATOR) != 0) {
                            break;

                        } else {
                            die("missing close brace");
                        }
                    }

                    parseOperationIntoExpression(es, os);
                }

                if (es->isEmpty()) {
                    // @REPORT warn empty expression ex: ()
                    die("empty expression\n");
                    return null;

                } else if (es->length != 1) {
                    // @REPORT error leftover operands ex: (4 + 4 4)
                    die("leftover operands");
                    return null;
                }

                currentParent->children->push(es->pop());

            } break;

            // parse function call or grouping of stuff
            case ')': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '(') break;

                    parseOperationIntoExpression(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open parens\n");
                    break;
                }

                if ((os->peek()->flags & NF_PUNCTUATOR) != 0) {
                    os->pop(); // discard open parens if it's just used to group

                } else {
                    parseOperationIntoExpression(es, os);
                }
            } break;

            // reduce-parse array indexer or array literal
            case ']': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '[') break;

                    parseOperationIntoExpression(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open bracket\n");
                    break;
                }

                parseOperationIntoExpression(es, os);

            } break;

            // reduce-parse a struct literal or code block
            case '}': {
                // this is the end of a closure, so set the current one to the old's parent
                closure = closure->parent;

                // also do this for the parent nodes we are keep track of.
                currentParent = oldParent;

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
                const auto node = resolveOperatorOrPunctuatorNode(tokens, i);

                // this is the beginning of a new closure.
                if (tt == '{' && (node->flags & NF_PUNCTUATOR) != 0) {
                    const auto parent = closure;
                    closure = (Closure*) pMalloc(sizeof (Closure));
                    closure->name = ""; // @TODO
                    closure->parent = parent;
                    closure->table = new Table<const char, Value>();

                    oldParent = currentParent;

                    currentParent->children->push(node);
                    currentParent = node;

                } else {
                    // it's a normal operator, for which precedence and associativity is relevant.
                    // resolve that stuff before adding the operator to the stack.
                    while (canPopAndApply(os, node)) parseOperationIntoExpression(es, os);

                    os->push(node);
                }
            } break;
        }

        i++;
    }

    return program;
}

ASTNode* Parser :: parse(Array<Token>* tokens) {
    return shuntingYard(tokens);
}

