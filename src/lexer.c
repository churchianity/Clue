
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "clue.h"
#include "util.h"

/**
 * Given a string |buffer| return an array of Token(s).
 * We are expecting the string to be null-terminated.
 */
Token* tokenize(char* buffer) {
    char c;
    char* tk;
    TokenTypeEnum tt;
    bool bad;
    unsigned int tl;

    unsigned int tc = 0;
    unsigned int capacity = CLUE_INITIAL_TOKEN_ARRAY_SIZE;
    Token* tokens = pmalloc(sizeof (Token) * capacity);

    unsigned int line = 1;
    unsigned int column = 1;

    while ((c = *buffer++) != '\0') {
        bad = false;

        if (c == '\n') {
            column = 1;
            line++;
            continue;

        } else if (c == ' ') {
            column++;
            continue;

        } else if (c == '\t') {
            column += 4; // @TODO replace this with a 'tab' counter? how else to do column count for tabs
            continue;

        } else if (isalpha(c)) {
            tt = TT_SYMBOL;

            tl = 1;
            do {
                if (!(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')) { // @TODO enforce camelCase?
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
            bool hasRadixPoint = false;
            do {
                if (!(isdigit(*buffer) || *buffer == '.')) {
                    break;

                } else if (*buffer == '.') { // only one '.' is allowed in a numeric literal.
                    if (hasRadixPoint) {
                        // bad = true;
                        // there is probably no syntactically valid thing of the form (numeric literal)(.)(something else)
                        // however, i don't know if it's better to report this error as a 'bad token' or as a mistaken use of the
                        // descent/ownership operator - (saying NUMERIC_LITERAL.property)
                        // how do I tell if the user meant to descend into the numeric literal (which you can't do), or just added an
                        // erroneous decimal point?
                        break;
                    }

                    hasRadixPoint = true;
                    tl++;

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
            bad = true;
            do {
                if (*buffer == quotemark) {
                    buffer++; // increment past the last quotemark
                    bad = false; // if we found a closing quotemark, the string is probably valid
                    break;

                } else {
                    tl++;
                }
            } while (*buffer++ != '\0');

            tk = pmalloc(sizeof (char) * tl);
            snprintf(tk, tl + 1, "%s", buffer - tl - 1);

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
                        tl = 2;
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
                        tl = 2;
                        snprintf(tk, 3, "%c=", c);
                        buffer++;
                        break;
                    }

                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                case '(':
                case ')':
                case ':':
                case '!':
                case '.':
                    tk = pmalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                default:
                    fprintf(stderr, "non-numeric, non-quote mark, non-alphabetic, invalid symbol character encountered :: %c\nskipping it...\n", c);
                    continue;
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

        tokens[tc++] = *newToken(line, column, tt, tk, bad);

        // increment the column based on the token's length
        column += tl;

        // it would be cool if we could move this inside the 'string' case of the lexer
        // but that makes the column counter for the position of the token be 2 greater than it should be
        // i think there's an ugly conditional either way
        if (tt == TT_STRING) {
            column += 2;
        }
    }

    tokens[tc] = *newToken(-1, -1, TT_SENTINEL, "END_OF_STREAM", false);

    return tokens;
}

