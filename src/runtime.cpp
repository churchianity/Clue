
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

    free(AST);
    free(codebuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    do {
        printf(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            fprintf(stderr, "error reading line from stdin and storing it in buffer %p\nskipping...\n", (void*) s);
            continue;
        }

        // fflush(stdin); // discard newline

        // super-secret interpreter options
        switch (s[0]) {
            case '.':
                printf("Bye!\n");
                return;

            case '`':
                Lexer::print();
                continue;

            case '/':
                Lexer::clear();
                // Lexer::print();
                continue;
        }

        Lexer::tokenize(s, "stdin");
        parse(Lexer::tokens);

    } while (1);
}

