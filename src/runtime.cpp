
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
    Token* tokens = tokenize(codebuffer, filename);
    // ASTNode* AST = parse(tokens);

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tPrinting lexed tokens @doIt...\n%s\n", _DIV);
        printTokens(tokens);
    #endif

    free(codebuffer);
}

/**
 * Enters the 'interactive' mode of the language which allows you to run/analyze code as you type it.
 */
void interactive() {
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    Token* tokens = NULL; // should retrieve this from the lexer incase we previously lexed a file
    ASTNode* AST = NULL; // ^

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

        if (tokens) free(tokens);
        tokens = tokenize(s, "stdin");
        // AST = parse(tokens);

        #if CLUE_DEBUG_LEVEL > 0
            printf("\n\tPrinting lexed tokens @doIt...\n%s\n", _DIV);
            printTokens(tokens);

            // printf("\n\tPrinting AST @doIt...\n%s\n", _DIV);
            //if (AST) {
            //    AST->traverse(AST, print);

            //} else {
            //    printf("The AST is null.\n");
            //}
        #endif

    } while (1);

    free(AST);
    free(tokens);
}

