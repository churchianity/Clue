
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "lexer.h"
#include "token.h"
#include "print.h"
#include "table.h"
#include "util.h"


/**
 *
 */
Lexer* lexer = NULL;

/**
 *
 */
void initLexer() {
    if (lexer) {
        return;
    }

    lexer = (Lexer*) pMalloc(sizeof (Lexer));

    lexer->files = newTable(10);
    lexer->tokenCount = 0;
    lexer->capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;
    lexer->token = NULL;
    lexer->tokens = (Token*) pMalloc(sizeof (Token) * lexer->capacity);
}

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
 * Given a string |buffer|, append to the lexer's |tokens| array.
 */
void tokenize(char* buffer, const char* filename) {
    bool prevTokenImport = false;
    TokenTypeEnum tt;

    u32 length = 0;
    bool bad;

    u32 line = 1;
    u32 column = 1;

    while (*buffer != '\0') {
        length = 1;
        bad = false;

        if (isAlpha(*buffer)) {
            /**
             * Symbols
             */
            tt = TT_SYMBOL;

            bool lastCharWasDigit = false;
            do {
                buffer++;

                if (!(isAlpha(*buffer) || isDigit(*buffer) || (*buffer == '_'))) {
                    break;
                }

                if (isDigit(*buffer)) {
                    lastCharWasDigit = true;

                } else if (lastCharWasDigit) {
                    // @TODO report linter error
                }

                length++;

            } while (*buffer != '\0');

        } else if (isDigit(*buffer)) {
            /**
             * Numeric
             */
            tt = TT_NUMERIC;

            // @TODO check if it's a zero, then it should be a fractional amount

            bool hasRadixPoint = false;
            do {
                buffer++;

                if (!isDigit(*buffer)) {
                    break;

                } else if (*buffer == '.') {
                    if (hasRadixPoint) {
                        bad = true;
                        break;
                    }

                    hasRadixPoint = true;
                }

                length++;

            } while (*buffer == '\0');

        } else if ((*buffer == '"') || (*buffer == '\'')) {
            /**
             * String
             */
            tt = TT_STRING;

            char quotemark = *buffer;
            bad = true;
            do {
                buffer++;

                if (*buffer == quotemark) {
                    length++; buffer++;
                    bad = false; // if we found a closing quotemark, the string is probably valid
                    break;
                }

                if (*buffer == '\\') {
                    // @TODO escape characters!
                }

                if (*buffer == '\n') {
                    continue;
                }

                length++;

            } while (*buffer != '\0');

        } else {
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
            tt = (TokenTypeEnum) *buffer;
            bad = false;

            // then check correctness:
            switch (*buffer) {
                case '\n': column = 1; line++; buffer++; continue;
                case '\t': column+= 4;         buffer++; continue;
                case ' ':  column++;           buffer++; continue;

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
                default:
                    fprintf(stderr, "invalid or unimplemented character encountered :: %c\nskipping it...", *buffer);

                    // @TODO report lexer error

                    buffer++;
                    continue;

                case '\0':
                    fprintf(stderr, "got null character while trying to lex an operator...\n");
                    print(lexer);
                    exit(1);
            }

            buffer += length;
        }

        add(newToken(filename, line, column, length, tt, read(buffer - length, length), bad));

        column += lexer->token->length;

        // handle import statement
        if (prevTokenImport) {
            if ((lexer->token->tt == TT_STRING) && (!lexer->token->bad)) {
                char* importFilePath = trimQuotes(lexer->token->tk, lexer->token->length);

                TableEntry* entry = lexer->files->lookup(lexer->files, importFilePath);

                if (entry) {
                    fprintf(stderr, "trying to import file that has already been imported... %s\n", importFilePath);

                } else {
                    lexer->files->insert(lexer->files, importFilePath, NULL);

                    tokenize(fileRead(importFilePath), importFilePath);
                }
            } else {
                // prev token is import, but our token for the path to the file to import isn't a proper string
                fprintf(stderr, "trying to import something that's not a good string:\n");
                print(lexer->token);
                exit(1);
            }
        }

        if (streq(lexer->token->tk, "import")) {
            prevTokenImport = true;

        } else {
            prevTokenImport = false;
        }
    }
}

