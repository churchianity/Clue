
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "reporter.h"
#include "clue.h"
#include "token.h"
#include "print.h"
#include "table.h"
#include "util.h"

Table* Lexer::files = newTable(10);

u32 Lexer::tokenCount = 0;
u32 Lexer::capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;

Token* Lexer::token = NULL;
Token* Lexer::tokens = (Token*) pMalloc(sizeof (Token) * Lexer::capacity);


void Lexer::print() {
    printf("lexer count: %u | capacity: %u\nfiles: ", Lexer::tokenCount, Lexer::capacity);
    print(Lexer::files);

    printf("\nlast token: "); print(Lexer::token); printf("\ntokens:\n");

    for (u32 i = 0; i < Lexer::tokenCount; i++) {
        print(Lexer::tokens + i);
    }
}

/**
 * Adds a token to the tokens array.
 */
void Lexer::add(Token* token) {
    Lexer::token = token;

    if (Lexer::capacity <= Lexer::tokenCount) {
        Lexer::capacity *= 2;
        Lexer::tokens = (Token*) pRealloc(Lexer::tokens, (sizeof (Token)) * Lexer::capacity);
    }

    Lexer::tokens[Lexer::tokenCount++] = *token;
}

/**
 * Given a string |buffer|, append to the lexer's |tokens| array.
 */
void Lexer::tokenize(char* buffer, const char* filename) {
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
                    Reporter::add({
                        "Non-digit character following a digit in identifier name",
                        MS_WARNING,
                        MC_LEXER
                    });
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
                    Lexer::print();
                    exit(1);
            }

            buffer += length;
        }

        Lexer::add(newToken(filename, line, column, length, tt, read(buffer - length, length), bad));

        column += Lexer::token->length;

        // handle import statement
        if (prevTokenImport) {
            if ((Lexer::token->tt == TT_STRING) && (!Lexer::token->bad)) {
                char* importFilePath = trimQuotes(Lexer::token->tk, Lexer::token->length);

                TableEntry* entry = Lexer::files->lookup(Lexer::files, importFilePath);

                if (entry) {
                    fprintf(stderr, "trying to import file that has already been imported... %s\n", importFilePath);

                } else {
                    Lexer::files->insert(Lexer::files, importFilePath, NULL);

                    tokenize(fileRead(importFilePath), importFilePath);
                }
            } else {
                // prev token is import, but our token for the path to the file to import isn't a proper string
                printf("bad\n"); ::print(Lexer::token);
                exit(1);
            }
        }

        if (streq(Lexer::token->tk, "import")) {
            prevTokenImport = true;

        } else {
            prevTokenImport = false;
        }
    }
}

