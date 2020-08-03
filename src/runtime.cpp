
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


float64 evalUnaryPlus(ASTNode* node) {
    return +eval(node->children + 0).number;
}

float64 evalBinaryAddition(ASTNode* node) {
    return eval(node->children + 0).number + eval(node->children + 1).number;
}

float64 evalUnaryMinus(ASTNode* node) {
    return -eval(node->children + 0).number;
}

float64 evalBinarySubtraction(ASTNode* node) {
    return eval(node->children + 0).number - eval(node->children + 1).number;
}

float64 evalMultiplication(ASTNode* node) {
    return eval(node->children + 0).number * eval(node->children + 1).number;
}

float64 evalDivision(ASTNode* node) {
    return eval(node->children + 0).number / eval(node->children + 1).number;
}

float64 evalModulus(ASTNode* node) {
    return remainder(eval(node->children + 0).number, eval(node->children + 1).number);
}

float64 evalExponentiation(ASTNode* node) {
    return pow(eval(node->children + 0).number, eval(node->children + 1).number);
}

Value evalOperator(ASTNode* node) {
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

/**
 * Finds a file in the project by name and loads it into a buffer then returns it.
 */
void clueFileRead(const char* filename) {
    char* codebuffer = fileRead(filename);
    Lexer::tokenize(codebuffer, filename);
    ASTNode* AST = parse(Lexer::tokens, Lexer::tokenCount);
    Reporter::flush();

    free(AST);
    free(codebuffer);
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
            case '.':
                print("Bye!\n");
                return;

            case '`':
                Reporter::flush();
                continue;

            case '/':
                Lexer::clear();
                traverse(AST, freeNode);
                continue;

            case '#':
                Lexer::print();
                continue;

            case '?':
                traverse(AST, print);
                continue;
        }

        tokens = Lexer::tokenize(s, "stdin", line);

        if (AST) {
            free(AST);
        }

        AST = parse(tokens, Lexer::tokenCount);

        printf("%.14g\n", eval(AST).number);

        Reporter::flush();

        line++; // do this last

    } while (true);
}

