
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clue.h"
#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"

/**
 * Evaluates a node.
 */
void evaluate(Node* node) {
    char *endptr = NULL;

    if (node->token->tt == TT_OPERATOR) {
        switch (node->token->tk[0]) {
            case '%':
            case '-':
            case '*':
            case '/':
            case '+':
                if (node->token->tk[1] == '+') {
                    // @TODO it's a pre/post increment operator
                    break;
                }
                break;

            default:
                break;
        }
    }

    if (strcmp(node->token->tk, "+") == 0) {
        printf("\nresult: %lu\n"
                , strtoul(node->children[0].token->tk, &endptr, 10) + strtoul(node->children[1].token->tk, &endptr, 10));
    }
}


/**
 * This will eventually become the 'interactive' mode of the language.
 */
void interactive() {
    char s[CLUE_INTERACTIVE_MODE_MAX_LINE_LENGTH];

    do {
        printf(">>> ");

        if (fgets(s, 80, stdin) == NULL) {
            perror("OOPSIE");
            break;
        }

        if (s[0] == '.') {
            printf("Bye!\n");
            return;
        }

        // TOKENIZING //
        Token* tokens = tokenize(s);

        printf("\nPrinting Tokens...\n\n");

        unsigned int i = 0;
        while (!(tokens[i].tt == TT_SENTINEL)) {
            tokens[i].print(&tokens[i]);
            i++;
        }

        /* PARSING //
        Node* root = parse(tokens);

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



