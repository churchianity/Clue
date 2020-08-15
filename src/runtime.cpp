
#include <math.h> // pow(), remainder()
                  // strtod

#include "clue.h"
#include "lexer.h"
#include "node.h"
#include "print.h"
#include "parser.h"
#include "reporter.h"
#include "runtime.h"
#include "string.h"
#include "token.h"
#include "util.h"


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

static inline Value evalOperator(ASTNode* node) {
    Value v;

    switch ((int) node->token->tt) {
        case '+':
            if (node->childrenCount == 1) {
                v.number = evalUnaryPlus(node);

            } else {
                v.number = evalBinaryAddition(node);
            }

            break;

        case '-':
            if (node->childrenCount == 1) {
                v.number = evalUnaryMinus(node);

            } else {
                v.number = evalBinarySubtraction(node);
            }

            break;

        case '*': v.number = evalMultiplication(node); break;
        case '/': v.number = evalDivision(node); break;
        case '%': v.number = evalModulus(node); break;

        case TT_EXPONENTIATION: v.number = evalExponentiation(node); break;

        case '~': v.number = evalBitwiseNot(node); break;
        case '!': v.number = evalNegation(node); break;

        default:
            v.string = "not implemented yet, sorry!\n";
    }

    return v;
}

Value eval(ASTNode* node) {
    Value v;

    switch (node->token->tt) {
        case TT_SYMBOL:
            v.string = "symbol!"; break;

        case TT_NUMERIC:
            v.number = strtod(node->token->tk, null); break;

        case TT_STRING:
            v.string = node->token->tk; break;

        default: v = evalOperator(node);
    }

    return v;
}

void doIt(char* codeBuffer, const char* filename) {
    Lexer::tokenize(codeBuffer, filename);
    print(eval(parse(Lexer::tokens, Lexer::tokenCount)));
    free(codeBuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    u32 line = 1;

    Token* tokens = null;
    ASTNode* AST = null;

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
                traverse(AST, freeNode);
                continue;

            case '#': // show state of the lexer
                Lexer::print();
                continue;

            case '?': // show state of the AST
                traverse(AST, print);
                continue;

            case '$': // run eval() on the AST
                print(eval(AST));
                continue;

            case '\n':
                continue;

            case 'L':
                print("\033[2J\033[H");
        }

        tokens = Lexer::tokenize(s, "stdin", line);
        AST = parse(tokens, Lexer::tokenCount);
        Reporter::flush();

        line++; // do this last

    } while (true);
}

