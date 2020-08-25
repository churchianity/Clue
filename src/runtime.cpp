
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
    return ~ (u64) eval(node->children + 0)->number;
}

static inline float64 evalNegation(ASTNode* node) {
    return !eval(node->children + 0)->number;
}

static inline float64 evalUnaryPlus(ASTNode* node) {
    return +eval(node->children + 0)->number;
}

static inline float64 evalBinaryAddition(ASTNode* node) {
    return eval(node->children + 0)->number + eval(node->children + 1)->number;
}

static inline float64 evalUnaryMinus(ASTNode* node) {
    return -eval(node->children + 0)->number;
}

static inline float64 evalBinarySubtraction(ASTNode* node) {
    return eval(node->children + 0)->number - eval(node->children + 1)->number;
}

static inline float64 evalMultiplication(ASTNode* node) {
    return eval(node->children + 0)->number * eval(node->children + 1)->number;
}

static inline float64 evalDivision(ASTNode* node) {
    return eval(node->children + 0)->number / eval(node->children + 1)->number;
}

static inline float64 evalModulus(ASTNode* node) {
    return remainder(eval(node->children + 0)->number, eval(node->children + 1)->number);
}

static inline float64 evalExponentiation(ASTNode* node) {
    return pow(eval(node->children + 0)->number, eval(node->children + 1)->number);
}

static Table<const char, Value>* global = new Table<const char, Value>(10);

static inline void evalAssignment(ASTNode* node) {
    global->insert((node->children + 0)->token->tk, (node->children + 0)->token->length, eval(node->children + 1));
}

static inline Value* evalOperator(ASTNode* node) {
    Value* v = (Value*) pMalloc(sizeof (Value));

    switch ((int) node->token->tt) {
        case '+':
            if (node->childrenCount == 1) {
                v->number = evalUnaryPlus(node);

            } else {
                v->number = evalBinaryAddition(node);
            }

            break;

        case '-':
            if (node->childrenCount == 1) {
                v->number = evalUnaryMinus(node);

            } else {
                v->number = evalBinarySubtraction(node);
            }

            break;

        case '*': v->number = evalMultiplication(node); break;
        case '/': v->number = evalDivision(node); break;
        case '%': v->number = evalModulus(node); break;

        case '=':
        case TT_COLON_EQUALS:
            evalAssignment(node);
            break;

        case TT_EXPONENTIATION: v->number = evalExponentiation(node); break;

        case '~': v->number = evalBitwiseNot(node); break;
        case '!': v->number = evalNegation(node); break;

        default:
            v->string = "not implemented yet, sorry!\n";
    }

    return v;
}

Value* eval(ASTNode* node) {
    Value* v = null;

    switch (node->token->tt) {
        case TT_SYMBOL:
            v = global->lookup(node->token->tk, node->token->length)->value;
            break;

        case TT_NUMERIC:
            v = (Value*) pMalloc(sizeof (Value));
            v->type = VT_NUMBER;
            v->number = strtod(node->token->tk, null);
            break;

        case TT_STRING:
            v = (Value*) pMalloc(sizeof (Value));
            v->type = VT_STRING;
            v->string = node->token->tk;
            break;

        default:
            v = evalOperator(node);
            break;
    }

    return v;
}

void doIt(char* codeBuffer, const char* filename) {
    Lexer::tokenize(codeBuffer, filename);
    print(eval(parse(Lexer::tokens)));
    free(codeBuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    #define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 160
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    u32 line = 1;

    ASTNode* AST = null;

    Array<char>* history = new Array<char>(10);

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

            case '?': // show state of the AST
                traverse(AST, print);
                continue;

            case '$': // run eval() on the AST
                print(eval(AST));

                print("\nGlobal Scope:\n");
                global->traverse(
                    [] (const char* key) {
                        print("%s : ", key);
                    },
                    [] (Value* v) {
                        print(v);
                    });
                continue;

            case '\n':
                continue;
        }

        history->push(s);

        Lexer::tokenize(s, "stdin", line);
        AST = parse(Lexer::tokens);
        Reporter::flush();

        line++; // do this last

    } while (true);
}

