
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


const char* evalString(ASTNode* node) {
    return node->token->tk;
}

float64 evalNumeric(ASTNode* node) {
    return strtod(node->token->tk, null);
}

float64 evalBinaryAddition(ASTNode* node) {
    return evalNumeric(node->children + 0) + evalNumeric(node->children + 1);
}

float64 evalBinarySubtraction(ASTNode* node) {
    return evalNumeric(node->children + 0) - evalNumeric(node->children + 1);
}

void evalOperator(ASTNode* node) {

}

/**
 *
 */
void eval(ASTNode* node) {
    switch (node->token->tt) {
        case TT_SYMBOL: // go lookup its value in a table
        case TT_NUMERIC: evalNumeric(node); break;
        case TT_STRING: evalString(node); break;
        default: evalOperator(node); break;
    }
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

        Reporter::flush();

        line++; // do this last

    } while (true);
}

