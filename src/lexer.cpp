
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "clue.h"
#include "util.h"


/**
 * Pre-processor statement to copy the contents of a file into the tokenizer.
 */
//@STRING
static Token* import(char* buffer) {
    return NULL;
}

/**
 *
 */
//@STRING
Token* lexSymbol(char* buffer, const char* filename, u32 line, u32 column) {
    u32 length = 0;

    while (*buffer != '\0') {
        if (!(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')) {
            break;
        }

        length++;
        buffer++;
    }

    if (length == 0) {
        return NULL; // @TODO report lexer error
    }

    return newToken(filename, line, column, length, TT_SYMBOL, read(buffer - length, length), false);
}

/**
 *
 */
//@STRING
Token* lexNumeric(char* buffer, const char* filename, u32 line, u32 column) {
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

    if (length == 0) {
        return NULL; // @TODO report lexer error
    }

    return newToken(filename, line, column, length, TT_NUMERIC, read(buffer - length, length), bad);
}

/**
 *
 */
//@STRING
Token* lexString(char* buffer, const char* filename, u32 line, u32 column) {
    // increment past what we *assume* is the opening quotemark
    char quotemark = *buffer++;

    u32 length = 0;
    bool bad = true;
    do {
        if (*buffer == quotemark) {
            buffer++;
            bad = false; // if we found a closing quotemark, the string is probably valid
            break;
        }

        length++;
    } while (*buffer++ != '\0');

    if (bad) {
        // @TODO report lex error
    }

    return newToken(filename, line, column, length, TT_STRING, read(buffer - length, length), bad);
}

/**
 * Operators & punctuators, all of which have their own token-type.
 * Single-char operators like '=' and '+' have their codepoint as their type,
 * while 2&3-char operators have a defined type in token.h
 *
 * Invalid characters read by the lexer are necessarily also handled here,
 * because the key indicator that a token is an operator is usually that it
 * is *not* one of the other types.
 */
//@STRING
Token* lexOperator(char* buffer, const char* filename, u32 line, u32 column) {

    // assume the token is its own type, is valid, and of length 1...
    TokenTypeEnum tt = (TokenTypeEnum) *buffer;
    bool bad = false;
    u32 length = 1;

    // then check correctness:
    switch (*buffer) {
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
                    case '+': tt = TT_INCREMENT;            break;
                    case '-': tt = TT_DECREMENT;            break;
                    case '*': tt = TT_EXPONENTIATION;       break;
                    case '/': tt = TT_SINGLE_LINE_COMMENT;  break;
                    case '=': tt = TT_EQUALITY;             break;
                    case '&': tt = TT_LOGICAL_AND;          break;
                    case '|': tt = TT_LOGICAL_OR;           break;
                    default:                                break;
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
                    case '>': tt = TT_GREATER_THAN_OR_EQUAL;    break;
                    case '<': tt = TT_LESS_THAN_OR_EQUAL;       break;
                    case '*': tt = TT_TIMES_EQUALS;             break;
                    case '+': tt = TT_PLUS_EQUALS;              break;
                    case '-': tt = TT_MINUS_EQUALS;             break;
                    case '/': tt = TT_DIVIDE_EQUALS;            break;
                    case '=': tt = TT_EQUALITY;                 break;
                    case '&': tt = TT_BITWISE_AND_EQUALS;       break;
                    case '|': tt = TT_BITWISE_OR_EQUALS;        break;
                    case '!': tt = TT_NOT_EQUALS;               break;
                    case '%': tt = TT_MODULO_EQUALS;            break;
                    case '~': tt = TT_BITWISE_NOT_EQUALS;       break;
                    case ':': tt = TT_COLON_EQUALS;             break;
                    case '^': tt = TT_BITWISE_XOR_EQUALS;       break;
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
            fprintf(stderr, "invalid or unimplemented character encountered :: %c\n", *buffer); fflush(stderr);
            return NULL;
    }

    return newToken(filename, line, column, length, tt, read(buffer, length), bad);
}

/**
 * Given a string |buffer| return an array of Token(s).
 * We are expecting the string to be null-terminated.
 */
//@STRING
Token* tokenize(char* buffer, const char* filename) {
    u32 capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;
    Token* tokens = (Token*) pMalloc(sizeof (Token) * capacity);
    u32 tokenCount = 0;

    Token* token = NULL;

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
            token = lexSymbol(buffer, filename, line, column);

        } else if (isdigit(*buffer)) {
            token = lexNumeric(buffer, filename, line, column);

        } else if ((*buffer == '"') || (*buffer == '\'')) {
            token = lexString(buffer, filename, line, column);

        } else {
            token = lexOperator(buffer, filename, line, column);
        }

        // we have a token. add it to the array, realloc'ing as necessary
        if (capacity <= tokenCount) {
            capacity *= 2;
            tokens = (Token*) pRealloc(tokens, sizeof (Token) * capacity);
        }

        tokens[tokenCount++] = *token;

        if (token->tt == TT_STRING) {
            column += 2;
            buffer += 2;
        }

        column += token->length;
        buffer += token->length;
    }

    // make sure there's space before appending sentinel token...
    if (capacity <= tokenCount) {
        capacity *= 2;
        tokens = (Token*) pRealloc(tokens, sizeof (Token) * capacity);
    }

    tokens[tokenCount] = *newToken(filename, 0, 0, 13, TT_SENTINEL, "END_OF_STREAM", false);

    return tokens;
}

