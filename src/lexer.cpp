
#include "clue.h"
#include "lexer.h"
#include "string.h"
#include "token.h"
#include "table.h"
#include "util.h"
#include "print.h"
#include "reporter.h"


Table* Lexer::files = newTable(10);

u32 Lexer::tokenCount = 0;
u32 Lexer::capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;

Token* Lexer::token = NULL;
Token* Lexer::tokens = (Token*) pMalloc(sizeof (Token) * Lexer::capacity);

void Lexer::clear() {
    free(Lexer::files);

    for (u32 i = 0; i < Lexer::tokenCount; i++) {
        Lexer::print();

        print(&Lexer::tokens[i]); fflush(stdout);

        free(&Lexer::tokens[i]);
    }

    Lexer::files = newTable(10);

    Lexer::tokenCount = 0;
    Lexer::capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;

    Lexer::token = NULL;
    Lexer::tokens = (Token*) pMalloc(sizeof (Token) * Lexer::capacity);
}

void Lexer::print() {
    printf("lexer count: %u | capacity: %u\nfiles: ", Lexer::tokenCount, Lexer::capacity);

    u32 i = 0;
    TableEntry* entry = Lexer::files->entries[i];

    for (; i < Lexer::files->capacity; i++) {

        while (entry) {
            printf("%s   ", entry->key);

            entry = entry->next;
        }
    }

    printf("\nprev token: "); print(Lexer::token); printf("\ntokens:\n");

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
    // const char* beginning = buffer;
    bool prevTokenImport = false;

    Token* token = NULL;
    TokenTypeEnum tt;

    u32 length = 0;
    bool bad;

    u32 line = 1;
    u32 column = 1;

    while (*buffer != '\0') {
        token = (Token*) pMalloc(sizeof (Token));
        length = 1;
        bad = false;

        if (isAlpha(*buffer)) {
            /**
             * Symbols
             */
            tt = TT_SYMBOL;

            bool lastCharWasDigit = false;
            bool lastCharWasDigitLint = false;
            do {
                buffer++;

                if (!(isAlpha(*buffer) || isDigit(*buffer) || (*buffer == '_'))) {
                    break;
                }

                if (isDigit(*buffer)) {
                    lastCharWasDigit = true;

                } else {
                    if (!lastCharWasDigitLint && lastCharWasDigit) {
                        lastCharWasDigitLint = true;

                        Reporter::add(
                                MS_LINT,
                                "only alphabetical characters can follow a digit in an identifier name.",
                                NULL,
                                filename,
                                line,
                                column
                        );
                    }

                    lastCharWasDigit = false;
                }

                length++;

            } while (*buffer != '\0');

            token->symbol = (Symbol*) pMalloc(sizeof (Symbol));
            token->symbol->name = read(buffer - length, length);

        } else if (isDigit(*buffer)) {
            /**
             * Numeric
             */
            tt = TT_NUMERIC;

            bool hasRadixPoint = false;

            if (*buffer == '0') {
                switch (*(buffer + 1)) {
                    case 'x': case 'X':
                    case 'b': case 'B':
                    case 'o': case 'O':
                    case '.':

                    default:
                        // @TODO report error: leading zeroes can only be in the form '0x', '0b', '0o', or '0.'
                        break;
                }
            }

            do {
                buffer++;

                if (*buffer == '.') {
                    if (hasRadixPoint) {
                        bad = true;
                        // @TODO report something?
                        break;
                    }

                    hasRadixPoint = true;

                } else if (!isDigit(*buffer)) {
                    break;
                }

                length++;

            } while (*buffer != '\0');

            token->number = atof(read(buffer - length, length));

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

                } else if (*buffer == '\n') {
                    column = 1; line++; continue;

                } else if (*buffer == '\t') {
                    column += 4; continue; // @TODO robustness
                }

                length++;

            } while (*buffer != '\0');

            token->string = read(buffer - length, length);

        } else {
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
                    fprintf(stderr, "invalid or unimplemented character encountered :: %c, codepoint: %d...\n", *buffer, *buffer);
                    exit(1);

                case '\0':
                    fprintf(stderr, "got NULL character while trying to lex an operator...\n");
                    Lexer::print();
                    exit(1);
            }

            token->op = (Operator*) pMalloc(sizeof (Operator));
            token->op->name = read(buffer, length);

            buffer += length;
        } // end if else - we should have a token

        token->filename = filename;
        token->line = line;
        token->column = column;
        token->length = length;
        token->tt = tt;
        // the token's value should've been assigned above...
        token->bad = bad;

        Lexer::add(token);

        column += Lexer::token->length;

        // handle import statement
        if (prevTokenImport) {
            if ((Lexer::token->tt == TT_STRING) && (!Lexer::token->bad)) {
                char* importFilePath = trimQuotes(Lexer::token->string, Lexer::token->length);

                TableEntry* entry = Lexer::files->lookup(Lexer::files, importFilePath);

                if (entry) {
                    // @TODO report warn: trying to import file that has already been imported: 'filename'
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

        if ((tt == TT_STRING) && streq(Lexer::token->string, "import")) {
            prevTokenImport = true;

        } else {
            prevTokenImport = false;
        }
    }
}

