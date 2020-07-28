
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


/**
 * Finds a file in the project by name and loads it into a buffer then returns it.
 */
void clueFileRead(const char* filename) {
    char* codebuffer = fileRead(filename);
    Lexer::tokenize(codebuffer, filename);
    ASTNode* AST = parse(Lexer::tokens);

    traverse(AST, print);

    free(AST);
    free(codebuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    ASTNode* AST = null;

    do {
        printf(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            die("error reading line from stdin and storing it in buffer %p\n", s);
        }

        // super-secret interpreter options
        switch (s[0]) {
            case '.':
                printf("Bye!\n");
                return;

            case '`':
                Reporter::flush();
                continue;

            case '/':
                Lexer::clear();
                continue;

            case '#':
                Lexer::print();
                continue;

            case '?':
                traverse(AST, print);
                continue;
        }

        AST = null;
        Lexer::tokenize(s, "stdin");
        AST = parse(Lexer::tokens);
        Reporter::flush();

    } while (true);
}

