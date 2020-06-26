
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clue.h"
#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"

static const char* DIV = "================================================================================";

/**
 * Evaluates a node.
 */
void evaluate(Node* node) {

}



/**
 * This will eventually become the 'interactive' mode of the language.
 */
void interactive() {
    initSymbolTable();
    char s[CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH];

    do {
        memset(s, 0, sizeof (s));
        printf(">>> ");

        if (fgets(s, CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH, stdin) == NULL) {
            fprintf(stderr, "error reading line and storing it in buffer %p\nskipping...\n", (void*) s);
            continue;
        }

        if (s[0] == '.') {
            printf("Bye!\n");
            return;
        }

        // TOKENIZING //
        Token* tokens = tokenize(s);

        printf("%s\n\tPrinting tokens...\n%s\n\n", DIV, DIV);
        printTokens(tokens);

        // PARSING //
        parse(tokens);


        /*
        if (root) {
            printf("\nTraversing Nodes...\n\n");
            root->traverse(root, root->print);
        }

        // EVALUATION //
        // root->traverse(root, &evaluate);

        */
        // cleanup

        free(tokens);

    } while (1);
}

