
#include <stdio.h> // for fgets, fprintf, printf
#include <stdlib.h> // memset

#include "clue.h"
#include "token.h"
#include "node.h"
#include "lexer.h"
#include "print.h"
#include "parser.h"
#include "string.h"
#include "runtime.h"
#include "util.h"


/**
 *
 */
void clueFileRead(const char* filename) {
    char* codebuffer = fileRead(filename);
    tokenize(codebuffer, filename);
    // ASTNode* AST = parse(tokens);

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tPrinting lexer state @doIt...\n%s\n", _DIV);
        print(lexer);
    #endif

    free(codebuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    do {
        memset(s, 0, sizeof (s));
        printf(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == NULL) {
            fprintf(stderr, "error reading line from stdin and storing it in buffer %p\nskipping...\n", (void*) s);
            continue;
        }

        if (s[0] == '.') {
            printf("Bye!\n");
            break;
        }

        tokenize(s, "stdin");

        #if CLUE_DEBUG_LEVEL > 0
            printf("\n\tPrinting lexer state @doIt...\n%s\n", _DIV);
            print(lexer);
        #endif

    } while (1);
}

