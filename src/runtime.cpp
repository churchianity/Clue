
#include <math.h> // pow(), remainder()
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


static inline float64 evalBitwiseNot(ASTNode* node) {
    return ~ (u64) eval(node->children + 0).number;
}

static inline float64 evalNegation(ASTNode* node) {
    return !eval(node->children + 0).number;
}

static inline float64 evalUnaryPlus(ASTNode* node) {
    return +eval(node->children + 0).number;
}

static inline float64 evalBinaryAddition(ASTNode* node) {
    return eval(node->children + 0).number + eval(node->children + 1).number;
}

static inline float64 evalUnaryMinus(ASTNode* node) {
    return -eval(node->children + 0).number;
}

static inline float64 evalBinarySubtraction(ASTNode* node) {
    return eval(node->children + 0).number - eval(node->children + 1).number;
}

static inline float64 evalMultiplication(ASTNode* node) {
    return eval(node->children + 0).number * eval(node->children + 1).number;
}

static inline float64 evalDivision(ASTNode* node) {
    return eval(node->children + 0).number / eval(node->children + 1).number;
}

static inline float64 evalModulus(ASTNode* node) {
    return remainder(eval(node->children + 0).number, eval(node->children + 1).number);
}

static inline float64 evalExponentiation(ASTNode* node) {
    return pow(eval(node->children + 0).number, eval(node->children + 1).number);
}

static Table<const char, Value>* global = new Table<const char, Value>(10);

static inline void evalAssignment(ASTNode* node) {
    Value* valuePointer = (Value*) pMalloc(sizeof (Value));
    Value value = eval(node->children + 1);

    if (value.type == VT_NUMBER) {
        valuePointer->type = value.type;
        valuePointer->number = value.number;

    } else {
        valuePointer->type = value.type;
        valuePointer->string = value.string;
    }

    global->insert((node->children + 0)->token->tk, (node->children + 0)->token->length, valuePointer);
}

static inline Value evalOperator(ASTNode* node) {
    Value v;

    switch ((int) node->token->tt) {
        case '+':
            // @TODO string concat
            if (node->childrenCount == 1) {
                v.type = VT_NUMBER;
                v.number = evalUnaryPlus(node);

            } else {
                v.type = VT_NUMBER;
                v.number = evalBinaryAddition(node);
            }

            break;

        case '-':
            v.type = VT_NUMBER;

            if (node->childrenCount == 1) {
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

        case '!':
            v.type = VT_NUMBER;
            v.number = evalNegation(node);
            break;

        default:
            v.type = VT_STRING;
            v.string = "not implemented yet, sorry!\n";
            break;
    }

    return v;
}

Value eval(ASTNode* node) {
    Value v;

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

void eval(Program* program) {
    for (u32 i = 0; i < program->statements->length; i++) {
        print(eval(program->statements->data[i]));
    }
}

void doIt(char* codeBuffer, const char* filename) {
    Lexer::tokenize(codeBuffer, filename);
    Program* program = parse(Lexer::tokens);

    eval(program);

    free(codeBuffer);
    free(program);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    Program* program = null;

    #define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 160
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    u32 line = 1;

    do {
        print(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            die("error reading line from stdin and storing it in buffer %p\n", s);
        }

        // super-secret interpreter options
        switch (s[0]) {
            case '.': // exit the interpreter
                print("Bye!\n");
                return;

            case '`': // show all the messages
                Reporter::flush();
                continue;

            case '/': // delete everything
                Lexer::clear();
                /*
                traverse(AST,
                    [] (ASTNode* node) {
                        print(node); fflush(stdout);
                        free(node);
                    }
                );
                */
                continue;

            case '#': // show state of the lexer
                Lexer::print();
                continue;

            case '?': // print the whole thing
                print(program);
                continue;

            case '*': // show the state of the global scope
                global->traverse(
                    [] (const char* key) {
                        print("%s : ", key);
                    },
                    [] (Value* v) {
                        print(*v);
                    });
                continue;

            case '$': // re-run the whole program
                eval(program);
                continue;

            case '\n':
                continue;
        }

        Lexer::tokenize(s, "stdin", line);

        if (program) {
            free(program);
            program = null;
        }

        program = parse(Lexer::tokens);
        Reporter::flush();

        line++; // do this last

    } while (true);
}

