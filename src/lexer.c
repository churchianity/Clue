
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "token.h"
#include "clue.h"
#include "util.h"

/**
 * Given a string |buffer| return an array of Token(s).
 */
Token* tokenize(char* buffer) {
    char c;
    char* tk;
    TokenType tt;
    unsigned int tl;

    unsigned int tc = 0;
    unsigned int capacity = CLUE_INITIAL_TOKEN_ARRAY_SIZE;
    Token* tokens = pmalloc(sizeof (Token) * capacity);

    unsigned int line = 1;
    unsigned int column = 1;

    while ((c = *buffer++) != '\0') {
        if (c == '\n') {
            column = 1;
            line++;
            continue;

        } else if (c == ' ') {
            column++;
            continue;

        } else if (c == '\t') {
            column += 4; // @TODO replace this with a 'tab' counter?
            continue;

        } else if (isalpha(c)) {
            tt = SYMBOL;

            tl = 1;
            do {
                if (!(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')) {
                    break;

                } else {
                    tl++;
                }
            } while (*buffer++ != '\0');

            tk = pmalloc(sizeof (char) * tl);
            snprintf(tk, tl + 1, "%s", buffer - tl);

        } else if (isdigit(c)) {
            tt = NUMERIC;

            tl = 1;
            do {
                if (!(isdigit(*buffer) || *buffer == '.')) {
                    break;

                } else {
                    tl++;
                }
            } while (*buffer++ != '\0');

            tk = pmalloc(sizeof (char) * tl);
            snprintf(tk, tl + 1, "%s", buffer - tl);

        } else if (c == '"' || c == '\'') {
            tt = STRING;
            char quotemark = c;

            tl = 0;
            do {
                if (*buffer == quotemark) {
                    buffer++;
                    break;

                } else {
                    tl++;
                }
            } while (*buffer++ != '\0');

            tk = pmalloc(sizeof (char) * tl);
            snprintf(tk, tl + 1, "%s", buffer - tl - 1);

            // column += 2; // do this here or below?

        } else {
            tt = OPERATOR;
            tl = 1;

            switch (c) {
                case '=': // assignment operator
                case '&': // bitwise AND
                case '|': // bitwise OR

                    // check for the 2-length logical operators that start with the above chars
                    // '==' equality
                    // '&&' logical AND
                    // '||' logical OR
                    if (*(buffer + 1) == c) {
                        tk = pmalloc(3 * sizeof (char));
                        snprintf(tk, 3, "%c%c", c, *buffer++);
                        break;
                    }

                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                // arithmetic
                case '+':
                case '-':
                case '*':
                case '/':
                case '%':

                    // check for compound assignment
                    if (*(buffer + 1) == '=') {
                        tk = pmalloc(3 * sizeof (char));
                        snprintf(tk, 3, "%c=", c);
                        buffer++;
                        break;
                    }

                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                case ':':
                case '!':
                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                // non-numeric, non-quote mark, non-alphabetic, invalid symbol character encountered.
                default:
                    fprintf(stderr, "ur ugly\n");
            }
        }

        // we have a token. add it to the array, realloc'ing as necessary
        if (capacity < tc) {
            capacity *= 2;
            tokens = realloc(tokens, sizeof (Token) * capacity);

            if (!tokens) {
                fprintf(stderr, "failed to realloc tokens. exiting...\n");
                free(tokens); // <-- DO I NEED TO DO THIS??????
                exit(1);
            }
        }

        tokens[tc++] = (Token) {
            tk = tk,
            tt = tt,
            line = line,
            column = column
        };

        // increment the column based on the token's length
        column += strlen(tk);

        if (tt == STRING) {
            column += 2;
        }
    }

    // sentinel token for end of stream
    tokens[tc] = (Token) {
        tk = "END_OF_STREAM",
        tt = NO_OP,
        line = -1,
        column = -1
    };

    return tokens;
}

