
#include <stdio.h> // for fgets, fprintf, printf
#include <stdlib.h> // memset

#include "clue.h"
#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"
#include "string.h"
#include "runtime.h"


/**
 *
 */
void doIt(char* codeBuffer, const char* filename) {
    Token* tokens = tokenize(codeBuffer, filename);

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tPrinting lexed tokens @doIt...\n%s\n", _DIV);
        printTokens(tokens);
    #endif

    ASTNode* AST = parse(tokens);

    // CLEANUP //
    free(AST);
    free(tokens);
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
            fprintf(stderr, "error reading line and storing it in buffer %p\nskipping...\n", (void*) s);
            continue;
        }

        if (s[0] == '.') {
            printf("Bye!\n");
            return;
        }

        doIt(s, "stdin");

    } while (1);
}

