
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
        while (!(tokens[i].tt == NO_OP)) {
            printToken(tokens[i++]);
        }

        Node* root = parse(tokens);
        printf("traverse here lmao\n");
        // traverse here lmao

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

