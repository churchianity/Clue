
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "lexer.h"

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

