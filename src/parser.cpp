
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
        case ':':
        case TT_QUESTION_MARK_COLON:
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
        case TT_EXPONENTIATION:
        case ',':
        case '~':
        case '!':
        case TT_NOT:
        case '@':
        case '$':
        case '{':
        case '(':
        case '[':
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
        case TT_AND:
        case TT_OR:
        case TT_EQUALITY:
        case TT_NOT_EQUALS:
        case TT_AS:
            return OA_LEFT_TO_RIGHT;

        case '#':
        case TT_DO:
        case TT_IF:
        case TT_ELSEIF:
        case TT_ELSE:
        case TT_WHILE:
        case TT_RETURN:
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

        case TT_IF:
        case TT_ELSEIF:
        case TT_ELSE:
        case TT_DO:
        case TT_WHILE:
        case TT_RETURN:
            return -1;

        case '(':
        case '[':
        case '{':
        case ':':
        case TT_QUESTION_MARK_COLON:

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
            return 0;

        case ',':
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
            // I have no idea why, but changing this to 4 from 7 fixes a precedence bug with exponentiation
            // @TODO figure this out
            return 4;

        case '@':
        case '$':
        case '.':
            return 8;

        case TT_AS:
            return 9;

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
    }

    return false;
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
static void unwrapCommas(ASTNode* parent) {
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
}


void parseOperationIntoExpression(Array<ASTNode>* es, Array<ASTNode>* os, Scope* scope) {
    const auto node = os->pop();
    const s32 tt = node->token->tt;

    if (tokenTypeIsNullary(node->token->tt)) {
        es->push(node);
        return;

    } else if (tt == TT_IF || tt == TT_ELSEIF) {
        const auto top = es->peek();

        ASTNode* elseStatement = null;
        if (top->token->tt == TT_ELSEIF || top->token->tt == TT_ELSE) {
            elseStatement = es->pop();
        }

        const auto body = es->pop();
        const auto predicate = es->pop();

        if (!body) {
            // @REPORT if statement missing body
            die("if statement missing body\n");
        }
        if (!predicate) {
            // @REPORT if statement missing predicate
            die("if statement missing predicate\n");
        }

        node->children = new Array<ASTNode>(2);
        node->children->push(predicate);
        node->children->push(body);

        if (elseStatement != null) {
            node->children->push(elseStatement);
        }
    } else if (tt == TT_ELSE) {
        const auto top = es->peek();

        if (!top || top->token->tt != '{' || ((top->flags & NF_STRUCT_LITERAL) == NF_STRUCT_LITERAL)) {
            // @REPORT fatal
            die("else statement missing body/body isn't a code block.\n");
        }

        node->children = new Array<ASTNode>(1);
        node->children->push(es->pop());

    } else if (tt == TT_DO) {
        ASTNode* block = null;
        ASTNode* whileStatement = null;

        auto top = es->peek();
        if (!top || top->token->tt != '{' || ((top->flags & NF_STRUCT_LITERAL) == NF_STRUCT_LITERAL)) {
            // @REPORT fatal
            // @NOTE we fail to parse do-while statements at the moment, and they usually crash here.
            // (the while statement greedily grabs the code block for itself, and ends up on top of the stack).
            die("thing following a 'do' isn't a code block\n");

        } else {
            block = es->pop();
        }

        top = es->peek();
        if (top && top->token->tt == TT_WHILE) {
            node->children = new Array<ASTNode>(2);
            node->children->push(block);
            node->children->push(whileStatement);

        } else {
            node->children = new Array<ASTNode>(1);
            node->children->push(block);
        }
    } else if (tt == TT_WHILE) {
        const auto body = es->pop();
        const auto predicate = es->pop();

        if (predicate == null) {
            // body should be the predicate, and this while statement should be part of a do-while construct.
            node->children = new Array<ASTNode>(1);
            node->children->push(body);

        } else {
            node->children = new Array<ASTNode>(2);
            node->children->push(predicate);
            node->children->push(body);
        }
    } else if (tt == '(') {
        // 'grouping' operators should never appear here. So this is eitherL
        // 1. a named function call
        // 2. a named function declaration
        const auto block = es->pop();
        const auto args = es->pop();
        const auto name = es->pop();

        node->children = new Array<ASTNode>(3);
        node->children->push(name);
        node->children->push(args);
        node->children->push(block);

    } else if (tt == '[') {
        if ((node->flags & NF_INDEXER) == NF_INDEXER) {
            die("should be parsing an indexer expression, but can't yet.\n");

        } else {
            die("should be parsing an array literal, but can't yet\n");
        }

    } else if (tt == '{') {
        if ((node->flags & NF_STRUCT_LITERAL) == NF_STRUCT_LITERAL) {
            die("should be parsing a struct literal but can't yet.\n");

        } else {
            node->children = new Array<ASTNode>();

            u32 count = es->length;
            for (u32 i = 0; i < count; i++) {
                const auto top = es->peek();

                if (top->token->scope != scope) {
                    break;
                }

                node->children->push(es->pop());
            }

            node->children->reverse();
        }
    } else if ((node->flags & NF_UNARY) == NF_UNARY) {
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

// given an array of tokens and a position in the array which points to an operator,
// figure out what kind of operator it is, and return it as an ASTNode*
static ASTNode* resolveOperatorNode(Array<Token>* tokens, u32 i) {
    const auto node = (ASTNode*) pCalloc(sizeof (ASTNode));

    node->token = tokens->data[i];

    if (tokenTypeIsNullary(node->token->tt)) {
        return node;
    };

    if (i < 1 || tokenTypeIsOperator(tokens->data[i - 1]->tt)) {
        // we think it's a unary operator, but we could be wrong. check.
        switch ((s32) node->token->tt) {
            default:
                die("unexpected operator that we thought would be unary or a punctuator: %d\n", tokens->data[i]->tt);
                break;

            // can only be an empty group or function call w/ no args
            case ')':
                // @TODO it isn't obvious that this should be done. check.
                if (tokens->data[i - 1]->tt != '(') {
                    // @TODO report hanging close parens.
                    die("hanging close parens\n");
                }
                break;

            // can only be an empty array literal
            case ']':
                // @TODO it isn't obvious that this should be done. check.
                if (tokens->data[i - 1]->tt != '[') {
                    // @TODO report hanging close bracket.
                    die("hanging close bracket\n");
                }
                break;

            // can only be an empty dict literal
            case '}':
                // @TODO it isn't obvious that this should be done. check.
                if (tokens->data[i - 1]->tt != '{') {
                    // @TODO report hanging close brace.
                    die("hanging close brace\n");
                }
                break;

            // should be a 'grouping' operator.
            case '(':
                node->flags |= NF_GROUP;
                break;

            // should be an array literal.
            case '[': break;

            // should be a dict literal
            // unless it's the first token, or the previous is a semicolon, or the previous is a closing brace - then it's a code block.
           case '{': break;

            case TT_IF:
            case TT_FOR:
            case TT_DO:
            case TT_WHILE:
            case TT_ELSE:
            case TT_RETURN:
                // these aren't necessarily unary operators, but they can assume the position of one.
                // if we see one here, that's fine.
                // even the ones that are just 'normal' unary like 'else' or 'return' don't get the unary flag,
                // because they are handled explicitly.
                break;

            case '~':
            case '!': // @deprecate
            case TT_NOT:
            case '@':
            case '#':
            case '$':

            case '+':
            case '-':
                node->flags |= NF_UNARY;
                break;
        }
    } else if ((tokens->data[i]->tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        if (i < (tokens->length - 1)) {
            const auto next = tokens->data[i + 1];

            // if the next token is a colon or a brace, then it's a function declaration
            // otherwise it's an invocation
            if (!(next->tt == ':' || next->tt == '{')) {
                node->flags |= NF_CALL;
            }
        } else {
            // @REPORT fatal, unexpected end of input - possibly/probably a missing semicolon at the end of a function call which is the last line of the program.
            die("unexpected end of input\n");
        }
    } else if ((tokens->data[i]->tt == '[') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_INDEXER;

    } else if ((tokens->data[i]->tt == TT_INCREMENT) || (tokens->data[i]->tt == TT_DECREMENT)) {
        node->flags |= NF_UNARY;
        node->flags |= NF_POSTFIX;

    } else {
        // it's probably a normal binary operator.
        // certain punctuators and statements can trick the parser into thinking that they are binary if the
        // prev token is not an operator or a symbol, check for that here.
        // ie:
        //      "string"[0];
        //              ^ parser thinks we are indexing a string literal
        //      4(foo);
        //       ^ parser thinks we are calling a function named '4'
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

static ASTNode* shuntingYard(Array<Token>* tokens) {
    const auto es = new Array<ASTNode>();
    const auto os = new Array<ASTNode>();

    auto scope = (Scope*) pMalloc(sizeof (Scope));
    scope->name = "global";
    scope->parent = null;
    scope->table = Runtime_getGlobalSymbolTable();

    ASTNode* programRoot = (ASTNode*) pCalloc(sizeof (ASTNode));
    programRoot->children = new Array<ASTNode>();

    u32 i = 0;
    while (i < tokens->length) {
        const auto token = tokens->data[i];
        s32 tt = (s32) token->tt;

        token->scope = scope;

        switch (tt) {
            case ';': {
                // dump the stack - we should collect everything as a single expression.
                while (!os->isEmpty()) {
                    const auto top = os->peek();

                    if (top->token->scope != scope) {
                        // the operator is in a different scope than us, so we don't want to continue.
                        break;
                    }

                    parseOperationIntoExpression(es, os, scope);
                }

                if (es->isEmpty()) {
                    // @REPORT warn empty expression ex: ()
                    // die("empty expression\n");

                } else {
                    if (0) {
                        // @REPORT error leftover operands ex: (4 + 4 4)
                        die("leftover operands");
                    }
                }
            } break;

            case ')': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '(') break;

                    parseOperationIntoExpression(es, os, scope);
                }

                if (os->isEmpty()) {
                    die("missing open parens\n");
                }

                if ((os->peek()->flags & NF_CALL) == NF_CALL) {
                    // it's a function call.
                    // parseOperationIntoExpression(es, os, scope);

                } else {
                    // discard open parens if it's just used to group
                    os->pop();
                }
            } break;

            // reduce-parse array indexer or array literal
            case ']': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '[') break;

                    parseOperationIntoExpression(es, os, scope);
                }

                if (os->isEmpty()) {
                    // @REPORT
                    die("missing open bracket\n");
                }

                parseOperationIntoExpression(es, os, scope);

            } break;

            // reduce-parse a struct literal or code block
            case '}': {
                parseOperationIntoExpression(es, os, scope);

                // this is the end of a scope, so set the current one to the old's parent
                scope = scope->parent;

                if (i < (tokens->length - 1)) {
                    const s32 tt = tokens->data[i + 1]->tt;

                    // if the next token isn't one of these types, we should perform as many
                    // stack reductions as we can, because we know there can't be any
                    // outstanding operations that we cannot resolve.
                    //
                    // this is true because a closing brace, like a semicolon, is something that
                    // many expressions/statements cannot cross.
                    if (tt == TT_ELSEIF || tt == TT_ELSE || tt == TT_WHILE) {
                        break;
                    }

                    while (!os->isEmpty()) {
                        if (os->peek()->token->tt == '{') break;

                        parseOperationIntoExpression(es, os, scope);
                    }
                }
            } break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC: {
                const auto node = (ASTNode*) pCalloc(sizeof (ASTNode));
                node->token = token;
                es->push(node);

            } break;

            case '{': {
                // we have a new namespace/scope, make it before doing anything else.
                const auto parent = scope;
                scope = (Scope*) pMalloc(sizeof (Scope));
                // scope->name = ""; // @TODO should be assigned later.
                scope->parent = parent;
                scope->table = new Table<const char, Value>();
            }
            default: {
                const auto node = resolveOperatorNode(tokens, i);

                // resolve precedence and associativity by re-arranging the stacks before pushing.
                while (canPopAndApply(os, node)) parseOperationIntoExpression(es, os, scope);

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

        parseOperationIntoExpression(es, os, scope);
    }

    if (es->isEmpty()) {
        // @REPORT warn
        // die("empty program\n");
    }

    for (u32 i = 0; i < es->length; i++) {
        programRoot->children->push(es->data[i]);
    }

    delete es;
    delete os;

    return programRoot;
}

ASTNode* Parser_parse(Array<Token>* tokens) {
    return shuntingYard(tokens);
}

