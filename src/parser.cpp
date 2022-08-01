
#include "array.hpp"
#include "node.h"
#include "parser.h"
#include "reporter.h"
#include "print.h"
#include "message.h"
#include "runtime.h"
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
        case '=':
        case ':':
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
            return OA_NONE;

        case '~':
        case '!':
        case '@':
        case '$':
            return OA_RIGHT_TO_LEFT;

        case '+':
        case '-':
            if ((node->flags & NF_UNARY) == NF_UNARY) {
                return OA_RIGHT_TO_LEFT;
            }
        case TT_LOGICAL_AND:
        case TT_LOGICAL_OR:
        case TT_LOGICAL_XOR:
        case '*':
        case '/':
        case TT_DOUBLE_FORWARD_SLASH:
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:
        case '.':
        case TT_EXPONENTIATION:
            return OA_LEFT_TO_RIGHT;

        case '(':
            return OA_NONE;

        default:
            die("trying to get associativity of unknown operator type: %u\n", node->token->tt);
            return OA_NONE;
    }
}

static inline s8 precedence(ASTNode* node) {
    switch ((s32) node->token->tt) {
        case '#':
            return -2;

        case '(':
            return 0;

        case '=':
        case ':':
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
        case TT_LOGICAL_XOR:
            return 2;

        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case '<':
        case TT_LESS_THAN_OR_EQUAL:
        case '>':
        case TT_GREATER_THAN_OR_EQUAL:
            return 3;

        case '+':
        case '-':
            if ((node->flags & NF_UNARY) == NF_UNARY) {
                return 6;
            }
            return 4;

        case '*':
        case '/':
        case TT_DOUBLE_FORWARD_SLASH:
        case '%':
        case '&':
        case '|':
        case '^':
        case TT_RIGHT_SHIFT:
        case TT_LEFT_SHIFT:
            return 5;

        case '~':
        case '!':
            return 6;

        case TT_EXPONENTIATION:
            return 7;

        case '@':
        case '$':
        case '.':
            return 8;

        default:
            die("trying to get precedence of unknown operator type: %u\n", node->token->tt);
            return 0;
    }
}

static inline bool canPopAndApply(Array<ASTNode>* os, ASTNode* node) {
    if (os->isEmpty()) return false;

    const auto top = os->peek();

    if (associativity(node) == OA_LEFT_TO_RIGHT) {
        return precedence(node) <= precedence(top);

    } else if (associativity(node) == OA_RIGHT_TO_LEFT) {
        return precedence(node) < precedence(top);

    } else {
        print(node);
        print(top);
        if (precedence(node) == precedence(top)) {
            die("non-associative sub-expression\n\n");
        }

        return false;
    }
}

// lists of things (comma-separated) get parsed like:
// ex: 4, 3, 2, 1
// into:
//      ,
//      |- 4
//      |- ,
//         |- 3
//         |- ,
//            |- 2
//            |- 1
//
// but it's usually more convienent to refer to a list as a single comma node with a variable number of children, like:
//      ,
//      |- 4
//      |- 3
//      |- 2
//      |- 1
//
// this function converts the former into the latter
static ASTNode* unwrapCommas(ASTNode* parent) {
    const auto children = new Array<ASTNode>();
    const auto stack = new Array<ASTNode>();
    auto cursor = parent;

    while (true) {
        children->push(cursor->children->data[0]);

        if (cursor != null && cursor->children->data[1] && cursor->children->data[1]->token->tt == ',') {
            stack->push(cursor->children->data[1]);

            cursor = cursor->children->data[1];

        } else {
            children->push(cursor->children->data[1]);
            break;
        }
    }

    while (!stack->isEmpty()) {
        delete stack->pop();
    }

    delete parent->children;
    parent->children = children;
    return parent;
}

static void parseOperationIntoExpression(Array<ASTNode>* es, Array<ASTNode>* os) {
    const auto node = os->pop();

    if ((node->flags & NF_UNARY) == NF_UNARY) {
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

        if (node->token->tt == ',') {
            unwrapCommas(node);
        }
    }

    es->push(node);
}

// given an array of tokens and a position in the array which points to an operator,
// figure out what kind of operator it is, and return it as an ASTNode*
static ASTNode* resolveOperatorNode(Array<Token>* tokens, u32 i) {
    const auto node = (ASTNode*) pCalloc(1, sizeof (ASTNode));

    node->token = tokens->data[i];
    const auto tt = tokens->data[i]->tt;

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) { // @NOTE we can remove the 'i < 1' if we change how pre-processor statements are done
        // we think it's a unary operator, but we could be wrong. check.
        switch ((s32) node->token->tt) {
            default:
                die("unexpected operator that we thought would be unary or a punctuator: %d\n", tokens->data[i]->tt);
                break;

            case '(':
                node->flags |= NF_GROUP;
                break;

            case '~':
            case '!':
            case '@':
            case '#':
            case '$':

            case '+':
            case '-':
                node->flags |= NF_UNARY;
                break;
        }
    } else if ((tt == '[') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_INDEXER;

    } else if ((tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_CALL;

    } else {
        // it's probably a normal binary operator.
        // certain punctuators and statements can trick the parser into thinking that they are binary if the
        // prev token is not an operator or a symbol, check for that here.
        // ie:
        //      4(foo);
        //       ^ parser thinks we are calling a function named '4'
        //
        //      "string"[0];
        //              ^ parser thinks we are indexing a string literal
        //                maybe should actually be legal code...
        //
        //       if 1 { ... }
        //            ^ parser thinks the opener of a code block is a binary operator,
        //              the code is actually valid
        //      ...
        //      }
        //      if true { ... }
        //      ^ parser thinks 'if' is binary because the closing brace that precedes it
        //        isn't technically considered an operator. the code is valid.
        //
        const auto prevToken = tokens->data[i - 1];

        if (!(tokenTypeIsStatement(tokens->data[i]->tt) && tokenTypeIsPunctuator(prevToken->tt))) {
            switch ((s32) tokens->data[i]->tt) {
                case '(':
                    if (prevToken->tt == TT_NUMERIC || prevToken->tt == TT_STRING) {
                        // @REPORT
                        die("Weird looking function call. Did you miss an operator before the open paren?\n");
                    }
                    break;
                case '[':
                    if (prevToken->tt == TT_NUMERIC) {
                        // @REPORT
                        die("Trying to index a numeric type.\n");

                    } else if (prevToken->tt == TT_STRING) {
                        // @REPORT
                        die("Trying to index a string literal.\n");
                    }
                    break;

                case '{':
                    break;

                default:
                    // check if it's actually a binary operator, or a mistake.
                    // exceptions can be put above.
                    if (tokenTypeBinaryness(tokens->data[i]->tt) < 1) {
                        die("expecting binary operator, but got op: %d", tokens->data[i]->tt);
                    }
                    break;
            }
        }
    }

    return node;
}

// https://en.wikipedia.org/wiki/Shunting-yard_algorithm
// the shunting yard algorithm is used to convert infix notation into reverse polish notation,
// and happens to do a pretty good job of constructing an abstract syntax tree as it does this.
// we use to it to construct expressions, which are code section that resolve to some value.
// examples of expressions:
//  -   4 + 2
//  -   print(27*(9/3))
//  -   goo(foo(x?.bar, array[p + i*2]))
//  -   sin(max(2,3)/3*PI)
//  -   baz(foo(x?.bar, array[p + i*2]))
//
static ASTNode* shuntingYard(Array<Token>* tokens, u32 startIndex, u32 endIndex) {
    const auto es = new Array<ASTNode>();
    const auto os = new Array<ASTNode>();

    u32 i = startIndex;
    while (i < endIndex) {
        const auto token = tokens->data[i];
        s32 tt = (s32) token->tt;

        switch (tt) {
            case ')': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '(') break;

                    parseOperationIntoExpression(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open parens\n");
                }

                if ((os->peek()->flags & NF_GROUP) == NF_GROUP) {
                    // discard open parens if it's just used to group
                    os->pop();
                }
            } break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC: {
                const auto node = (ASTNode*) pCalloc(1, sizeof (ASTNode));
                node->token = token;
                es->push(node);
            } break;

            default: {
                const auto node = resolveOperatorNode(tokens, i);

                // resolve precedence and associativity by re-arranging the stacks before pushing.
                while (canPopAndApply(os, node)) parseOperationIntoExpression(es, os);

                os->push(node);
            } break;
        }

        i++;
    }

    while (!os->isEmpty()) {
        const auto node = os->peek();
        s32 tt = (s32) node->token->tt;

        if (tt == '(' && ((node->flags & NF_CALL) == 0)) {
            // @REPORT
            die("missing close paren");

        } else if (tt == '[') {
            // @REPORT
            die("missing close bracket");

        } else if (tt == '{') {
            // @REPORT
            die("missing close brace");
        }

        parseOperationIntoExpression(es, os);
    }

    if (es->isEmpty()) {
        // @REPORT warn
        die("empty expression\n");
    }

    const auto expression = es->pop();
    for (u32 i = 0; i < es->length; i++) {
        expression->children->push(es->data[i]);
    }

    delete es;
    delete os;

    return expression;
}

ASTNode* Parser_parse(Array<Token>* tokens) {
    ASTNode* programRoot = (ASTNode*) pCalloc(1, sizeof (ASTNode));
    programRoot->children = new Array<ASTNode>();

    auto scope = (Scope*) pMalloc(sizeof (Scope));
    scope->name = "__global";
    scope->parent = null;
    scope->table = Runtime_getGlobalSymbolTable();

    u32 lastExpressionParseIndex = 0;
    u32 i = 0;
    while (i < tokens->length) {
        const auto token = tokens->data[i];
        s32 tt = (s32) token->tt;

        switch(tt) {
            case ';': {
                const auto node = shuntingYard(tokens, lastExpressionParseIndex, i);
                if (node != null) {
                    programRoot->children->push(node);
                }
                lastExpressionParseIndex = ++i;
                continue;
            } break;
        }

        i++;
    }

    return programRoot;
}

