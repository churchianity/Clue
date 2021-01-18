
#include <stdlib.h> // free
#include <math.h> // pow(), remainder(), floor()
                  // strtod

#include "lexer.h"
#include "node.h"
#include "parser.h"
#include "print.h"
#include "reporter.h"
#include "runtime.h" // eval() - weird self-dependency
#include "table.hpp"
#include "types.h"
#include "value.h"


static inline s32 fToInt(double f) {
    return (s32) floor(f + 0.5);
}

static inline double evalBitwiseNot(ASTNode* node) {
    return ~fToInt(eval(node->children->data[0]).number);
}

static inline u32 evalBitwiseAnd(ASTNode* node) {
    return fToInt(eval(node->children->data[0]).number) & fToInt(eval(node->children->data[1]).number);
}

static inline u32 evalBitwiseOr(ASTNode* node) {
    return fToInt(eval(node->children->data[0]).number) | fToInt(eval(node->children->data[1]).number);
}

static inline u32 evalBitwiseXor(ASTNode* node) {
    return fToInt(eval(node->children->data[0]).number) ^ fToInt(eval(node->children->data[1]).number);
}

static inline u32 evalLeftShift(ASTNode* node) {
    return fToInt(eval(node->children->data[0]).number) << fToInt(eval(node->children->data[1]).number);
}

static inline u32 evalRightShift(ASTNode* node) {
    return fToInt(eval(node->children->data[0]).number) >> fToInt(eval(node->children->data[1]).number);
}

static inline double evalNegation(ASTNode* node) {
    return !eval(node->children->data[0]).number;
}

static inline double evalUnaryPlus(ASTNode* node) {
    return +eval(node->children->data[0]).number;
}

static inline double evalBinaryAddition(ASTNode* node) {
    return eval(node->children->data[0]).number + eval(node->children->data[1]).number;
}

static inline double evalUnaryMinus(ASTNode* node) {
    return -eval(node->children->data[0]).number;
}

static inline double evalBinarySubtraction(ASTNode* node) {
    return eval(node->children->data[0]).number - eval(node->children->data[1]).number;
}

static inline double evalMultiplication(ASTNode* node) {
    return eval(node->children->data[0]).number * eval(node->children->data[1]).number;
}

static inline double evalDivision(ASTNode* node) {
    return eval(node->children->data[0]).number / eval(node->children->data[1]).number;
}

static inline double evalModulus(ASTNode* node) {
    return remainder(eval(node->children->data[0]).number, eval(node->children->data[1]).number);
}

static inline double evalExponentiation(ASTNode* node) {
    return pow(eval(node->children->data[0]).number, eval(node->children->data[1]).number);
}

static inline bool evalLogicalAnd(ASTNode* node) {
    return eval(node->children->data[0]).number && eval(node->children->data[1]).number;
}

static inline bool evalLogicalOr(ASTNode* node) {
    return eval(node->children->data[0]).number || eval(node->children->data[1]).number;
}

static inline bool evalGreaterThanOrEqual(ASTNode* node) {
    return eval(node->children->data[0]).number >= eval(node->children->data[1]).number;
}

static inline bool evalLessThanOrEqual(ASTNode* node) {
    return eval(node->children->data[0]).number <= eval(node->children->data[1]).number;
}

static inline bool evalEquals(ASTNode* node) {
    return eval(node->children->data[0]).number == eval(node->children->data[1]).number;
}

// @TODO move me somewhere not dumb!
static Table<const char, Value>* global = new Table<const char, Value>();

static inline void evalAssignment(ASTNode* node) {
    Value* valuePointer = (Value*) pMalloc(sizeof (Value));
    Value value = eval(node->children->data[1]);

    if (value.type == VT_NUMBER) {
        valuePointer->type = value.type;
        valuePointer->number = value.number;

    } else {
        valuePointer->type = value.type;
        valuePointer->string = value.string;
    }

    global->insert((node->children->data[0])->token->tk, (node->children->data[0])->token->length, valuePointer);
}

static inline Value evalOperator(ASTNode* node) {
    Value v;

    switch ((int) node->token->tt) {
        case TT_IMPORT: // imports are a pre-processor, and have already been run...
            v.type = VT_STRING;
            v.string = "";
            break;

        case ':':
            // @TODO append type information
            v = *global->lookup(node->token->tk, node->token->length)->value;
            break;

        case '+':
            // @TODO string concat
            if (node->children->length == 1) {
                v.type = VT_NUMBER;
                v.number = evalUnaryPlus(node);

            } else {
                v.type = VT_NUMBER;
                v.number = evalBinaryAddition(node);
            }

            break;

        case '-':
            v.type = VT_NUMBER;

            if (node->children->length == 1) {
                v.number = evalUnaryMinus(node);

            } else {
                v.number = evalBinarySubtraction(node);
            }

            break;

        case '*':
            v.type = VT_NUMBER;
            v.number = evalMultiplication(node);
            break;

        case '/':
            v.type = VT_NUMBER;
            v.number = evalDivision(node);
            break;

        case '%':
            v.type = VT_NUMBER;
            v.number = evalModulus(node);
            break;

        case '=':
        case TT_COLON_EQUALS:
            evalAssignment(node);
            v.type = VT_STRING;
            v.string = "";
            break;

        case TT_EXPONENTIATION:
            v.type = VT_NUMBER;
            v.number = evalExponentiation(node);
            break;

        case '~':
            v.type = VT_NUMBER;
            v.number = evalBitwiseNot(node);
            break;

        case '&':
            v.type = VT_NUMBER;
            v.number = evalBitwiseAnd(node);
            break;

        case '|':
            v.type = VT_NUMBER;
            v.number = evalBitwiseOr(node);
            break;

        case '^':
            v.type = VT_NUMBER;
            v.number = evalBitwiseXor(node);
            break;

        case TT_LEFT_SHIFT:
            v.type = VT_NUMBER;
            v.number = evalLeftShift(node);
            break;

        case TT_RIGHT_SHIFT:
            v.type = VT_NUMBER;
            v.number = evalRightShift(node);
            break;

        case '!':
            v.type = VT_NUMBER;
            v.number = evalNegation(node);
            break;

        case TT_LOGICAL_AND:
            v.type = VT_NUMBER;
            v.number = evalLogicalAnd(node);
            break;

        case TT_LOGICAL_OR:
            v.type = VT_NUMBER;
            v.number = evalLogicalOr(node);
            break;

        case TT_GREATER_THAN_OR_EQUAL:
            v.type = VT_NUMBER;
            v.number = evalGreaterThanOrEqual(node);
            break;

        case TT_LESS_THAN_OR_EQUAL:
            v.type = VT_NUMBER;
            v.number = evalLessThanOrEqual(node);
            break;

        case TT_EQUALITY:
            v.type = VT_NUMBER;
            v.number = evalEquals(node);
            break;

        case TT_NOT_EQUALS:
            v.type = VT_NUMBER;
            v.number = !evalEquals(node);
            break;

        //@TODO
        case TT_PLUS_EQUALS:
        case TT_MINUS_EQUALS:
        case TT_TIMES_EQUALS:
        case TT_DIVIDE_EQUALS:
        case TT_BITWISE_AND_EQUALS:
        case TT_BITWISE_OR_EQUALS:
        case TT_BITWISE_XOR_EQUALS:
        case TT_LEFT_SHIFT_EQUALS:
        case TT_RIGHT_SHIFT_EQUALS:
        case TT_EXPONENTIATION_EQUALS:
        default:
            v.type = VT_STRING;
            v.string = "not implemented yet, sorry!\n";
            break;
    }

    return v;
}

Value eval(ASTNode* node) {
    Value v;
    v.type = VT_STRING;
    v.string = "";

    if (!node) return v;

    switch (node->token->tt) {
        case TT_SYMBOL:
            v = *global->lookup(node->token->tk, node->token->length)->value;
            break;

        case TT_NUMERIC:
            v.type = VT_NUMBER;
            v.number = strtod(node->token->tk, null);
            break;

        case TT_STRING:
            v.type = VT_STRING;
            v.string = node->token->tk;
            break;

        default:
            v = evalOperator(node);
            break;
    }

    return v;
}

void eval(Array<ASTNode>* program) {
    program->forEach(
        [] (ASTNode* statement) {
            print(eval(statement));
        }
    );
}

void deleteEverything(Array<ASTNode>* program) {
    if (!program) return;

    for (s32 i = 0; i < program->length; i++) {
        traverse(program->data[i],
            [] (ASTNode* node) {
                pFree(node);
            }
        );

        program->pop();
    }
}


Array<ASTNode>* program = null;


void doIt(char* codeBuffer, const char* filename) {
    Lexer::tokenize(codeBuffer, filename);
    program = parse(Lexer::tokens);
    eval(program);
    pFree(codeBuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    const u32 CLUE_SANDBOX_MODE_MAX_LINE_LENGTH = 160;
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    u32 line = 1;

    do {
        print(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            die("error reading line from stdin and storing it in buffer %p\n", (void*) s);
        }

        // super-secret interpreter options
        switch (s[0]) {
            case '.':
                print("Bye!\n");
                return;

            case '/':
                print("deleting everything...\n");
                Lexer::clear();
                deleteEverything(program);
                program = null;
                continue;

            case '#':
                print("printing the state of the lexer...\n");
                Lexer::tokens->forEach(print);
                continue;

            case '&': // @TODO strangeness
                print("printing the whole program text...\n");
                Reporter::rebuild("samples/variables.clue");
                continue;

            case '?': {
                if (program != null) {
                    print("printing the AST...\n");
                    program->forEach([] (ASTNode* statement) {
                        traverse(statement, [] (ASTNode* node) {
                            if (node->children != null) print(node);
                        });
                    });
                }
            } continue;

            case '*': {
                print("printing the global namespace...\n");
                global->traverse(
                    [] (const char* key) {
                        print("%s : ", key);
                    },
                    [] (Value* v) {
                        print(*v);
                    });
            } continue;

            case '$':
                print("re-running the program...\n");
                eval(program);
                continue;
        }

        Lexer::tokenize(s, "stdin", line);
        program = parse(Lexer::tokens);
        eval(program);
        Reporter::flush();

        line++; // do this last

    } while (true);
}

