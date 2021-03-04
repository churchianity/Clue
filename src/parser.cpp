

#include "array.hpp"
#include "node.h"
#include "parser.h"
#include "print.h"
#include "token.h"


// this is the file where we turn an array of tokens into an abstract syntax tree.

static Array<ASTNode>* es = null;
static Array<ASTNode>* os = null;
static Closure* closure = null;
static ASTNode* oldParent = null;
static ASTNode* currentParent = null;
static ASTNode* outstandingStatement = null;
static ASTNode* program = null;


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

        case ':':
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
        case '@':
        case '$':
        case ',':
        case TT_EXPONENTIATION:

        case '(':
            if ((node->flags & NF_CALL) != NF_CALL) {
                return OA_NONE;
            }
        case '[':
            if ((node->flags & NF_INDEXER) != NF_INDEXER) {
                return OA_NONE;
            }
            return OA_RIGHT_TO_LEFT;

        default:
            die("trying to get associativity of unknown operator type: %u\n", node->token->tt);
            return OA_NONE;
    }
}

static inline u8 precedence(ASTNode* node) {
    switch ((s32) node->token->tt) {
        case TT_IF:
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
            // I have no idea why, but changing this to 4 from 7 fixes a precedence bug with exponentiation
            // @TODO figure this out
            return 4;

        case TT_INCREMENT:
        case TT_DECREMENT:
            if ((node->flags & NF_POSTFIX) == NF_POSTFIX) {
                return 6;
            }

        case '(':
            if ((node->flags & NF_CALL) != NF_CALL) {
                return 0;
            }
        case '[':
        case '{':
            if ((node->flags & NF_STRUCT_LITERAL) != NF_STRUCT_LITERAL) {
                return 0;
            }
            return 0;

        case '@':
        case '#':
        case '$':
        case '.':
        case ':':
        case TT_IMPORT:
        case TT_DO:

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

void parseOperationIntoExpression(Array<ASTNode>* es, Array<ASTNode>* os) {
    const auto node = os->pop();
    s32 tt = node->token->tt;

    if (tt == TT_IF) {
        node->children = new Array<ASTNode>(3);

        const auto predicate = es->pop();

        if (predicate == null) {
            // @TODO report if statement missing predicate
            die("if statement missin predicate\n");
            return;

        } else {
            node->children->push(predicate);
        }

        node->children->push(currentParent);
        outstandingStatement = node;
        return;

    } else if ((node->flags & NF_CALL) == NF_CALL) {
        die("should be parsing a function call operation, but can't yet.\n");
        return;

    } else if (tt == '[') {
        if ((node->flags & NF_INDEXER) == NF_INDEXER) {
            die("should be parsing an indexer expression, but can't yet.\n");

        } else {
            die("should be parsing an array literal, but can't yet\n");
        }
        return;

    } else if (tt == '{') {
        if ((node->flags & NF_STRUCT_LITERAL) == NF_STRUCT_LITERAL) {
            die("should be parsing a struct literal but can't yet.\n");

        } else {

            return;
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

static void setupNewCodeBlock(ASTNode* block) {
    block->children = new Array<ASTNode>();

    // add this code block as a child of the encompassing block
    currentParent->children->push(block);

    // switch-a-roo
    oldParent = currentParent;
    currentParent = block;

    // create and configure a new closure, assign to the current closure.
    const auto parent = closure;
    closure = (Closure*) pMalloc(sizeof (Closure));
    closure->name = ""; // @TODO
    closure->parent = parent;
    closure->table = new Table<const char, Value>();

    // resolve precedence and associativity by re-arranging the stacks
    while (canPopAndApply(os, block)) parseOperationIntoExpression(es, os);
}

// given an array of tokens and a position in the array which points to an operator,
// figure out what kind of operator it is, and put it on the operator stack.
static void resolveOperatorNode(Array<Token>* tokens, u32 i) {
    const auto node = (ASTNode*) pCalloc(sizeof (ASTNode));

    node->token = tokens->data[i];

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
            case '(': break;

            // should be an array literal.
            case '[': break;

            // should be a dict literal
            // unless it's the first token, or the previous is a semicolon - then it's a code block.
           case '{':
                if (i == 0 || tokens->data[i - 1]->tt == ';') {
                    setupNewCodeBlock(node);
                    return;
                }
                break;

            case TT_IF:
            case TT_WHILE:
            case TT_FOR:
                // lies. these can have many children, but take the position of a unary operator
                // we pretend they are unary, but add more children later anyway.

            case TT_DO:
            case TT_ELSE:
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
    } else if ((tokens->data[i]->tt == '(') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_CALL;

    } else if ((tokens->data[i]->tt == '[') && (tokens->data[i - 1]->tt == TT_SYMBOL)) {
        node->flags |= NF_INDEXER;

    } else if ((tokens->data[i]->tt == TT_INCREMENT) || (tokens->data[i]->tt == TT_DECREMENT)) {
        node->flags |= NF_UNARY;
        node->flags |= NF_POSTFIX;

    } else {
        // it's probably a normal binary operator.
        // certain punctuators can trick the parser into thinking that they are binary if the
        // prev token is not an operator or a symbol, check for that here.
        // ie:
        //      "string"[0];
        //              ^ parser thinks we are indexing a string literal
        //      4(foo);
        //       ^ parser thinks we are calling a function named '4'
        //
        //       if 1 { ... }
        //            ^ parser thinks the opener of a code block is a binary operator,
        //              this is the only example where the code is actually valid
        //
        const auto prevToken = tokens->data[i - 1];
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
                setupNewCodeBlock(node);
                return;

            default:
                // check if it's actually a binary operator, or a mistake.
                // exceptions can be put above.
                if (tokenTypeBinaryness(tokens->data[i]->tt) < 1) {
                    die("expecting binary operator, but got op: %d", tokens->data[i]->tt);
                }
                break;
        }
    }

    // resolve precedence and associativity by re-arranging the stacks before pushing.
    while (canPopAndApply(os, node)) parseOperationIntoExpression(es, os);

    os->push(node);
}

static ASTNode* shuntingYard(Array<Token>* tokens) {
    es = new Array<ASTNode>();
    os = new Array<ASTNode>();

    closure = (Closure*) pMalloc(sizeof (Closure));
    closure->name = "global";
    closure->parent = null;
    closure->table = new Table<const char, Value>();

    oldParent = null;

    currentParent = (ASTNode*) pCalloc(sizeof (ASTNode));
    currentParent->children = new Array<ASTNode>();

    outstandingStatement = null;

    program = currentParent;

    u32 i = 0;
    while (i < tokens->length) {
        const auto token = tokens->data[i];
        s32 tt = (s32) token->tt;

        token->closure = closure;

        switch (tt) {
            case ';': {
                // dump the stack - we should collect everything as a single expression.
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
                        if ((os->peek()->flags & NF_STRUCT_LITERAL) != NF_STRUCT_LITERAL) {
                            // if it's a opening curly brace, and not a struct literal
                            // then it's a code block, and we want to skip the rest of
                            // the stack, since an expression can't cross a block boundary.
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
                    print(es->pop());
                    print(es->pop());
                    print(es->pop());
                    die("leftover operands");
                    return null;
                }

                currentParent->children->push(es->pop());

            } break;

            case ')': {
                while (!os->isEmpty()) {
                    if (os->peek()->token->tt == '(') break;

                    parseOperationIntoExpression(es, os);
                }

                if (os->isEmpty()) {
                    die("missing open parens\n");
                    break;
                }

                if ((os->peek()->flags & NF_CALL) == NF_CALL) {
                    // it's a function call.
                    parseOperationIntoExpression(es, os);

                } else {
                    // discard open parens if it's just used to group
                    os->pop();
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
                if (oldParent == null) {
                    // @REPORT
                    die("hanging closing brace\n");
                }

                if (outstandingStatement) {
                    // we might need to re-arrange some things.
                    switch ((s32) outstandingStatement->token->tt) {
                        case TT_IF:
                            oldParent->children->data[oldParent->children->length - 1] = outstandingStatement;
                            break;
                    }

                    outstandingStatement = null;
                }

                // this is the end of a closure, so set the current one to the old's parent
                closure = closure->parent;

                // do the same for the parent nodes we are tracking.
                currentParent = oldParent;

            } break;

            case TT_SYMBOL:
            case TT_STRING:
            case TT_NUMERIC: {
                const auto node = (ASTNode*) pCalloc(sizeof (ASTNode));
                node->token = token;
                es->push(node);

            } break;

            default:
                resolveOperatorNode(tokens, i);
                break;
        }

        i++;
    }

    return program;
}

ASTNode* Parser :: parse(Array<Token>* tokens) {
    return shuntingYard(tokens);
}

