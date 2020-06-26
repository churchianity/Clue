
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clue.h"
#include "token.h"
#include "node.h"
#include "lexer.h"
#include "parser.h"
#include "runtime.h"

static const char* DIV = "================================================================================";

void evalPostfixTokens(Token* tokens) {
    unsigned int i = 0;

    while (tokens[i].tt != TT_SENTINEL) {
        // if (tokens[i].tt == TT_SYMBOL) {
        //     // check if the symbol has been previously defined and gets its value/otherwise define it or something
        // }

        if (tokens[i].tt == TT_OPERATOR) {
            evaluateOperator(&tokens[i]);
        }

        ++i;
    }
}

/**
 * Evaluates a node.
 */
void evaluateOperator(Token* token) {
    switch (token->tk[0]) {
        case '+':
            break;

        case '-':
            break;

        case '*':
            break;

        case '/':
            break;

        case '%':
            break;
    }
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
        Node* AST = parse(tokens);

        // CLEANUP //
        free(AST);

        free(tokens);

    } while (1);
}

