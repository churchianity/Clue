
#include <stdio.h>

#include "token.h"
#include "lexer.h"

/**
 *
 */
void debug() {
    char s[80];

    while (1) {
        // prompt and get string from user
        printf(">>> ");

        if (fgets(s, 80, stdin) == NULL) {
            perror("OOPSIE");
            break;
        }

        Token* tokens = tokenize(s);

        printf("Printing Tokens:\n");
        for (int i = 0; i < 10; i++) {
            printf("%2d,%2d | tt: %d, tk: %s\n", tokens[i].line, tokens[i].column, tokens[i].tt, tokens[i].tk);
        }
    }
}

/**
 *
 */
int main(int argc, char* argv[]) {
    debug();

    return 0;
}

