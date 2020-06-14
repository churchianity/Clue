
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"

/**
 *
 */
void debug() {
    char s[80];

    while (1) {
        printf(">>> ");

        if (fgets(s, 80, stdin) == NULL) {
            perror("OOPSIE");
            break;
        }

        Token* tokens = tokenize(s);

        printf("Printing Tokens:\n");

        int i = 0;
        while (!(tokens[i].tt == TT_NO_OP)) {
            tokens[i].print(&tokens[i]);
            i++;
        }

        Node* root = parse(tokens);
        if (root) {
            printf("traverse here lmao\n");
            traverse(root);
        }



        free(tokens);
    }
}

/**
 *
 */
int main(int argc, char* argv[]) {
    debug();

    return 0;
}

