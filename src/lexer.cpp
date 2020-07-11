
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "clue.h"
#include "util.h"


/**
 * Reads |length| characters from |buffer| into a newly allocated buffer and returns it.
 * Appends the null character, so the returned string is |length| + 1 in size.
 */
static char* read(char* buffer, u32 length) {
    char* tk = (char*) pMalloc(sizeof (char) * length + 1);
    snprintf(tk, length + 1, "%s", buffer - length);

    return tk;
}

/**
 * Pre-processor statement to copy the contents of a file into the tokenizer.
 */
static Token* import(char* buffer) {
    return NULL;
}

/**
 *
 */
Token* lexSymbol(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 0;
    do {
        if (!(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')) {
            break;

        } else {
            length++;
        }
    } while (*buffer++ != '\0');

    return newToken(filename, line, column, length, TT_SYMBOL, read(buffer, length), false);
}

/**
 *
 */
Token* lexNumeric(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 0;
    bool bad = false;
    bool hasRadixPoint = false;
    do {
        if (!(isdigit(*buffer) || *buffer == '.')) {
            break;

        } else if (*buffer == '.') { // only one '.' is allowed in a numeric literal
            if (hasRadixPoint) {
                bad = true; // @TODO report lexer error
                break;
            }

            hasRadixPoint = true;
            length++;

        } else {
            length++;
        }
    } while (*buffer++ != '\0');

    return newToken(filename, line, column, length, TT_NUMERIC, read(buffer, length), bad);
}

/**
 *
 */
Token* lexString(char* buffer, const char* filename, u32 line, u32 column) {
    char quotemark = *buffer++; // increment past initial quotemark and save it

    u32 length = 0;
    bool bad = true;
    do {
        if (*buffer == quotemark) {
            buffer++; // increment past the last quotemark
            bad = false; // if we found a closing quotemark, the string is probably valid
            break;

        } else {
            length++;
        }
    } while (*buffer++ != '\0');

    if (bad) {
        // @TODO report lex error
    }

    char* tk = (char*) pMalloc(sizeof (char) * length + 1);
    snprintf(tk, length + 1, "%s", buffer - length - 1);

    return newToken(filename, line, column, length, TT_STRING, tk, bad);
}

/**
 * Operators & punctuators, all of which have their own token-type.
 * Single-char operators like '=' and '+' have their codepoint as their type,
 * while 2-char operators have a defined type in token.h
 *
 * Invalid characters read by the lexer are necessarily also handled here.
 */
Token* lexOperator(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 1;
    TokenTypeEnum tt;
    bool bad = false;
    char* tk = NULL;

    switch (*buffer) {
        case '=': // assignment operator
        case '&': // bitwise AND
        case '|': // bitwise OR
        case '^': // bitwise XOR
            tt = (TokenTypeEnum) *buffer;

            /**
             * check for...
             *
             * '==' equality
             * '&&' logical AND
             * '||' logical OR
             */
            if (*buffer == *(buffer + 1)) {
                switch (*buffer) {
                    case '=': tt = TT_EQUALS; break;
                    case '&': tt = TT_LOGICAL_AND; break;
                    case '|': tt = TT_LOGICAL_OR; break;
                }

                length = 2;
                tk = read(buffer, length);
                break;

            /**
             * '&=' bitwise AND compound assignment
             * '|=' bitwise OR compound assignment
             * '^=' bitwise XOR compound assignment
             */
            } else if (*(buffer + 1) == '=') {
                switch (*buffer) {
                    case '&': tt = TT_BITWISE_AND_ASSIGNMENT; break;
                    case '|': tt = TT_BITWISE_OR_ASSIGNMENT; break;
                    case '^': tt = TT_BITWISE_XOR_ASSIGNMENT; break;
                }

                length = 2;
                tk = read(buffer, length);
                break;
            }

            tk = read(buffer, length);
            break;

        // arithmetic
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':

        case '~': // bitwise NOT

        case ':': // colon operator
            tt = (TokenTypeEnum) *buffer;

            /**
             * arithmetic compound assignments...
             * '+='
             * '-='
             * '*='
             * '/='
             * '%='
             *
             * '~=' bitwise NOT compound assignment
             *
             * ':=' alternative assignment (explicit type inference)
             */
            if (*(buffer + 1) == '=') {
                length = 2;
                tk = read(buffer, length);
                buffer++;
                break;
            }

            tk = read(buffer, length);
            break;

        case '.':
        case '(':
        case ')':
            tt = (TokenTypeEnum) *buffer;
            tk = read(buffer, length);
            break;

        default:
            fprintf(stderr, "invalid character encountered :: %c\n", *buffer);
            bad = true;
            tt = (TokenTypeEnum) *buffer;
            tk = read(buffer, length);
            break;
    }

    return newToken(filename, line, column, length, tt, tk, bad);
}

/**
 * Given a string |buffer| return an array of Token(s).
 * We are expecting the string to be null-terminated.
 */
Token* tokenize(char* buffer, const char* filename) {
    u32 capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;
    Token* tokens = (Token*) pMalloc(sizeof (Token) * capacity);
    u32 tc = 0;

    Token* token = NULL; // holds the value of the token just lexed

    u32 line = 1;
    u32 column = 1;

    char c;
    while ((c = *buffer) != '\0') {
        if (c == '\n') {
            column = 1; line++; buffer++; continue;

        } else if (c == ' ') {
            column++; buffer++; continue;

        } else if (c == '\t') {
            // @TODO report lexer warning (& maybe add a tab counter (probably not)?)
            column += 4; buffer++; continue;

        } else if (isalpha(c)) {
            token = lexSymbol(buffer, filename, line, column);

        } else if (isdigit(c)) {
            token = lexNumeric(buffer, filename, line, column);

        } else if (c == '"' || c == '\'') {
            token = lexString(buffer, filename, line, column);

        } else {
            token = lexOperator(buffer, filename, line, column);
        }

        // we have a token. add it to the array, realloc'ing as necessary
        if (capacity <= tc) {
            capacity *= 2;
            tokens = (Token*) pRealloc(tokens, sizeof (Token) * capacity);
        }

        tokens[tc++] = *token;

        column += token->length;

        // a consequence of not including quotes in the 'value' of a string token means we have to perform this check here.
        if (token->tt == TT_STRING) {
            column += 2;
            buffer += 2;
        }

        buffer += token->length;
    }

    tokens[tc] = *newToken(filename, 0, 0, 13, TT_SENTINEL, "END_OF_STREAM", false);

    return tokens;
}

