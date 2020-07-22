
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
 *
 */
void clueFileRead(const char* filename) {
    char* codebuffer = fileRead(filename);
    Lexer::tokenize(codebuffer, filename);
    ASTNode* AST = parse(Lexer::tokens);

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tPrinting lexer state @doIt...\n%s\n", _DIV);
        Lexer::print();

        printf("\n\tPrinting AST state... \n%s\n", _DIV);
        print(AST);
        traverse(AST, print);
    #endif

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

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == NULL) {
            fprintf(stderr, "error reading line from stdin and storing it in buffer %p\nskipping...\n", (void*) s);
            continue;
        }

        if (s[0] == '.') {
            printf("Bye!\n");
            break;

        } else if (s[0] == '`') {
            Lexer::clear();
            Lexer::print();
            continue;
        }

        Lexer::tokenize(s, "stdin");
        ASTNode* AST = parse(Lexer::tokens);

        #if CLUE_DEBUG_LEVEL > 0
            printf("\n\tPrinting lexer state...\n%s\n", _DIV);
            Lexer::print();

            // printf("\n\tPrinting AST state... \n%s\n", _DIV);
            // traverse(AST, print);

            Reporter::flush();
        #endif

        free(AST);

    } while (1);
}

