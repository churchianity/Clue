
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "lexer.h"
#include "token.h"
#include "util.h"


/**
 *
 */
Lexer* lexer = (Lexer*) pMalloc(sizeof (Lexer));

/**
 * Adds a token to the tokens array.
 */
static inline void add(Token* token) {
    lexer->token = token;

    if (lexer->capacity <= lexer->tokenCount) {
        lexer->capacity *= 2;
        lexer->tokens = (Token*) pRealloc(lexer->tokens, (sizeof (Token)) * lexer->capacity);
    }

    lexer->tokens[lexer->tokenCount++] = *token;
}

/**
 * Runs when we encounter a "import" token.
 */
static inline void import() {

}

/**
 * |buffer| must be a string, the first character of which is alphabetical.
 */
static inline void lexSymbol(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 0;

    while (*buffer != '\0') {
        if (!(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')) {
            break;
        }

        length++;
        buffer++;
    }

    add(newToken(filename, line, column, length, TT_SYMBOL, read(buffer - length, length), false));
}

/**
 * |buffer| must be a string, the first character of which is a digit.
 */
static inline void lexNumeric(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 0;

    bool bad = false;
    bool hasRadixPoint = false;

    while (*buffer != '\0') {
        if (!(isdigit(*buffer) || (*buffer == '.'))) {
            break;

        } else if (*buffer == '.') { // only one '.' is allowed in a numeric literal
            if (hasRadixPoint) {
                bad = true; // @TODO report lexer error
                break;
            }

            hasRadixPoint = true;
        }

        length++;
        buffer++;
    }

    add(newToken(filename, line, column, length, TT_NUMERIC, read(buffer - length, length), bad));
}

/**
 * |buffer| must be a string, the first character of which is a single or double quote.
 */
static inline void lexString(char* buffer, const char* filename, u32 line, u32 column) {

    // increment past what we assume is the opening quotemark
    char quotemark = *buffer++;

    if (*buffer == '\0') { // there's a quotemark as the last character in the file/stream before \0
        fprintf(stderr, "Don't do that.\n"); return;
    }

    u32 length = 1;
    bool bad = true;

    do {
        length++;

        if (*buffer++ == quotemark) {
            bad = false; // if we found a closing quotemark, the string is probably valid
            break;
        }

    } while (*buffer != '\0');

    if (bad) {
        return; // @TODO report lexer error
    }

    add(newToken(filename, line, column, length, TT_STRING, read(buffer - length, length), bad));
}

/**
 * Operators & punctuators, all of which have their own token-type.
 * Single-char operators like '=' and '+' have their codepoint as their type,
 * while 2&3-char operators have a defined type in token.h
 *
 * Tokens which behave like operators but look like symbols, are considered symbols.
 *
 * Invalid characters read by the lexer are necessarily also handled here,
 * because the key indicator that a token is an operator is usually that it
 * is *not* one of the other types.
 */
static inline void lexOperator(char* buffer, const char* filename, u32 line, u32 column) {

    // assume the token is its own type, is valid, and of length 1...
    TokenTypeEnum tt = (TokenTypeEnum) *buffer;
    bool bad = false;
    u32 length = 1;

    // then check correctness:
    switch (*buffer) {
        case ';':
        case ',':
        case '.':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
            break;

        case '>':
        case '<':
        case '*':
            if (*(buffer + 1) == *buffer) {
                switch (*buffer) {
                    case '>': tt = TT_RIGHT_SHIFT; break;
                    case '<': tt = TT_LEFT_SHIFT; break;
                    case '*': tt = TT_EXPONENTIATION; break;
                }

                // @TODO right & left shift assignment & assignment by exponentiation

                length = 2;
            }

        case '+':
        case '-':
        case '/':
        case '=':
        case '&':
        case '|':
            if (*(buffer + 1) == *buffer) {
                switch (*buffer) {
                    case '+': tt = TT_INCREMENT;           break;
                    case '-': tt = TT_DECREMENT;           break;
                    case '*': tt = TT_EXPONENTIATION;      break;
                    case '/': tt = TT_SINGLE_LINE_COMMENT; break;
                    case '=': tt = TT_EQUALITY;            break;
                    case '&': tt = TT_LOGICAL_AND;         break;
                    case '|': tt = TT_LOGICAL_OR;          break;
                }

                length = 2;
            }

        case '!':
        case '%':
        case '~':
        case ':':
        case '^':
            if (*(buffer + 1) == '=') {
                switch (*buffer) {
                    case '>': tt = TT_GREATER_THAN_OR_EQUAL; break;
                    case '<': tt = TT_LESS_THAN_OR_EQUAL;    break;
                    case '*': tt = TT_TIMES_EQUALS;          break;
                    case '+': tt = TT_PLUS_EQUALS;           break;
                    case '-': tt = TT_MINUS_EQUALS;          break;
                    case '/': tt = TT_DIVIDE_EQUALS;         break;
                    case '=': tt = TT_EQUALITY;              break;
                    case '&': tt = TT_BITWISE_AND_EQUALS;    break;
                    case '|': tt = TT_BITWISE_OR_EQUALS;     break;
                    case '!': tt = TT_NOT_EQUALS;            break;
                    case '%': tt = TT_MODULO_EQUALS;         break;
                    case '~': tt = TT_BITWISE_NOT_EQUALS;    break;
                    case ':': tt = TT_COLON_EQUALS;          break;
                    case '^': tt = TT_BITWISE_XOR_EQUALS;    break;
                }

                length = 2;
            }

            break;

        // invalid or unimplemented single-chars
        case '#':
        case '@':
        case '$':
        case '`':
        case '?':
        case '\\':
        case '\0':
        default:
            fprintf(stderr, "invalid or unimplemented character encountered :: %c\nskipping it...", *buffer);
            // @TODO report lexer error
    }

    add(newToken(filename, line, column, length, tt, read(buffer, length), bad));
}

/**
 * Given a string |buffer|, append to the lexer's |tokens| array.
 * We are expecting the string to be null-terminated.
 */
//@STRING
void tokenize(char* buffer, const char* filename) {

    u32 line = 1;
    u32 column = 1;

    while (*buffer != '\0') {
        if (*buffer == '\n') {
            column = 1; line++; buffer++; continue;

        } else if (*buffer == ' ') {
            column++; buffer++; continue;

        } else if (*buffer == '\t') {
            column += 4; buffer++; continue;

        } else if (isalpha(*buffer)) {
            lexSymbol(buffer, filename, line, column);

        } else if (isdigit(*buffer)) {
            lexNumeric(buffer, filename, line, column);

        } else if ((*buffer == '"') || (*buffer == '\'')) {
            lexString(buffer, filename, line, column);

        } else {
            lexOperator(buffer, filename, line, column);
        }

        column += lexer->token->length;
        buffer += lexer->token->length;

        // handle import statement
        if (streq(lexer->token->tk, "import")) {
            import();
        }
    }
}

