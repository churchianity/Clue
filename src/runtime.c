
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"

void execute(const Node* node) {
    char *endptr;

    if (strcmp(node->token->tk, "+") == 0) {
        printf("\nresult lmao: %lu\n"
               , strtol(node->children[0].token->tk, &endptr, 10) + strtol(node->children[1].token->tk, &endptr, 10));
    }
}


/**
 *
 */
void debug() {
    char s[80];

    do {
        printf(">>> ");

        if (fgets(s, 80, stdin) == NULL) {
            perror("OOPSIE");
            break;
        }

        if (s[0] == '.') {
            return;
        }

        // TOKENIZING //
        Token* tokens = tokenize(s);

        printf("\nPrinting Tokens...\n\n");

        unsigned int i = 0;
        while (!(tokens[i].tt == TT_NO_OP)) {
            tokens[i].print(&tokens[i]);
            i++;
        }

        // PARSING //
         Node* root = parse(tokens);

        if (root) {
            printf("\nTraversing Nodes...\n\n");
            traverse(root, root->print);
        }

        // DO THR THING //
         traverse(root, &execute);

        free(tokens);

    } while (1);
}

/**
 *
 */
int main(int argc, char* argv[]) {
    debug(); return 0;
}

