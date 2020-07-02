
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "clue.h"
#include "util.h"


/**
 * Given a string |buffer| return an array of Token(s).
 * We are expecting the string to be null-terminated.
 */
Token* tokenize(char* buffer, char* filename) {
    char c;
    char* tk;
    TokenTypeEnum tt;
    bool bad;
    char tl;

    u64 tc = 0;
    u64 capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;
    Token* tokens = pMalloc(sizeof (Token) * capacity);

    u64 line = 1;
    u64 column = 1;

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
            // @TODO report lexer warning
            column += 4;
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

            // @TODO check convention of the full symbol and report appropriate lexer warnings

            tk = pMalloc(sizeof (char) * tl);
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
                        bad = true; // @TODO report lexer error
                        break;
                    }

                    hasRadixPoint = true;
                    tl++;

                } else {
                    tl++;
                }
            } while (*buffer++ != '\0');

            tk = pMalloc(sizeof (char) * tl);
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

            if (bad) {
                // @TODO report lex error
            }

            tk = pMalloc(sizeof (char) * tl);
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
                        tk = pMalloc(3 * sizeof (char));
                        tl = 2;
                        snprintf(tk, 3, "%c%c", c, *buffer++);
                        break;
                    }

                    tk = pMalloc(2 * sizeof (char));
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
                        tk = pMalloc(3 * sizeof (char));
                        tl = 2;
                        snprintf(tk, 3, "%c=", c);
                        buffer++;
                        break;
                    }

                    tk = pMalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                    tk = pMalloc(2 * sizeof (char));
                    snprintf(tk, 2, "%c", c);
                    break;

                default:
                    fprintf(stderr, "non-numeric, non-quote mark, non-alphabetic, invalid symbol character encountered :: %c\nskipping it...\n", c);
                    continue;
            }
        }

        // we have a token. add it to the array, realloc'ing as necessary
        if (capacity <= tc) {
            capacity *= 2; // @TODO I don't know what the optimal growth rate is
            tokens = pRealloc(tokens, sizeof (Token) * capacity);
        }

        tokens[tc++] = *newToken(filename, line, column, tl, tt, tk, bad);

        // increment the column counter based on the token's length
        column += tl;

        // @NOTE it would be cool if we could move this inside the 'string' case of the lexer
        if (tt == TT_STRING) {
            column += 2;
        }
    }

    tokens[tc] = *newToken(filename, -1, -1, 13, TT_SENTINEL, "END_OF_STREAM", false);

    return tokens;
}

