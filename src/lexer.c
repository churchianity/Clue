
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
    TokenTypeEnum tt;
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
            tt = TT_SYMBOL;

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
            tt = TT_NUMERIC;

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
            tt = TT_STRING;
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
            tt = TT_OPERATOR;
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

                case '.':
                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                default:
                    fprintf(stderr, "non-numeric, non-quote mark, non-alphabetic, invalid symbol character encountered.\n");
                    tk = pmalloc(10 * sizeof (char));
                    snprintf(tk, 10, "uhohstinky");
                    break;
            }
        }

        // we have a token. add it to the array, realloc'ing as necessary
        if (capacity < tc) {
            capacity *= 2; // @TODO I don't know what the optimal growth rate is
            tokens = realloc(tokens, sizeof (Token) * capacity);

            if (!tokens) {
                fprintf(stderr, "failed to realloc tokens. exiting...\n");
                exit(1);
            }
        }

        tokens[tc++] = *newToken(line, column, tt, tk);

        // increment the column based on the token's length
        column += tl;

        // it would be cool if we could move this inside the 'string' case of the lexer
        // but that makes the column counter for the position of the token be 2 greater than it should be
        if (tt == TT_STRING) {
            column += 2;
        }
    }

    // sentinel token for end of stream
    tokens[tc] = *newToken(-1, -1, TT_NO_OP, "END_OF_STREAM");

    return tokens;
}

