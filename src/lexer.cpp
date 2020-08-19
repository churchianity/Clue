
#include "clue.h"
#include "lexer.h"
#include "print.h"
#include "reporter.h"
#include "string.h"
#include "token.h"
#include "table.hpp"
#include "util.h"


Array<Token>* Lexer::tokens = new Array<Token>(CLUE_INITIAL_TOKEN_ARRAY_CAPACITY);


/**
 * @STATEFUL
 */
void Lexer :: clear() {
    delete Lexer::tokens;
    Lexer::tokens = new Array<Token>(CLUE_INITIAL_TOKEN_ARRAY_CAPACITY);
}

/**
 * @STATEFUL
 */
void Lexer :: print() {
    ::print("Lexer: count: %u | capacity: %u\nfiles: ", Lexer::tokens->size(), Lexer::tokens->capacity);

    Lexer::tokens->forEach(::print);

    ::print("\n");
}

struct Keyword {
    TokenTypeEnum tt;
};

static inline Keyword* keyword(TokenTypeEnum tt) {
    Keyword* kw = (Keyword*) pMalloc(sizeof (kw));

    kw->tt = tt;

    return kw;
}

static Table<const char, Keyword>* initKeywordTable() {
    auto t = new Table<const char, Keyword>(25);

    t->insert("import",     6, keyword(TT_IMPORT));
    t->insert("if",         2, keyword(TT_IF));
    t->insert("else",       4, keyword(TT_ELSE));
    t->insert("while",      5, keyword(TT_WHILE));

    return t;
}

/**
 * Given a string |buffer|, append to the lexer's |tokens| array.
 *
 * |_line| is sometimes necessary to provide, because you can
 * lex tokens from the same source in batches, like in the 'stdin' interpreter case,
 *
 * It's like partially lexing a file, going off and doing something else, then returning
 * and wanting to remember where you left off.
 *
 * In most (file read) cases it is not provided and defaults to 1.
 *
 * @STATEFUL
 */
Array<Token>* Lexer :: tokenize(char* buffer, const char* filename, u32 _line) {
    static auto keywords = initKeywordTable();
    static auto files = new Table<const char, void>(10); // names of all the files loaded so far

    static bool prevTokenImport = false;
    bool isSingleLineComment = false;

    Token* token = null;
    TokenTypeEnum tt;

    u32 line = _line;
    u32 column = 1;

    u32 length = 0;
    bool bad;

    while (*buffer != '\0') {

        // if it's not the null character, we (probably) have a valid token of atleast 1 in length
        length = 1;
        bad = false;

        if (isAlpha(*buffer)) {
            tt = TT_SYMBOL;

            bool lastCharWasDigit = false;
            do {
                buffer++;

                if (!(isAlpha(*buffer) || isDigit(*buffer) || (*buffer == '_'))) {
                    break;
                }

                if (isDigit(*buffer)) {
                    lastCharWasDigit = true;

                } else {
                    if (lastCharWasDigit) {
                        Reporter::add(L_FOLLOWING_DIGIT_IN_IDENTIFIER, null, filename, line, column + length);
                    }

                    lastCharWasDigit = false;
                }

                length++;

            } while (*buffer != '\0');

        } else if (isDigit(*buffer)) {
            tt = TT_NUMERIC;

            bool hasRadixPoint = false;

            /**
             *  handle 0-prefix special numerics...
             *
             *  0x / 0X : hexadecimal
             *  0o / 0O : octal
             *  0b / 0B : binary
             *  0.      : fractional decimal

            if (*buffer == '0') {
                switch (*(buffer + 1)) {
                    default:
                        // @REPORT 2
                        break;

                    case '.': // fractional decimal, we don't need to do anything special
                        break;

                    // octal constant. you can choose whether or not to be explicit about octal with 'o' or 'O'
                    // a leading zero that isn't one of the other cases has octal semantics by default
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    case 'o': case 'O':

                    case 'x': case 'X': // @TODO hexadecimal constant

                    case 'b': case 'B': // @TODO binary constant
                }
            }
             */

            do {
                buffer++;

                if (*buffer == '.') {
                    if (hasRadixPoint) {
                        bad = true;

                        // @REPORT 3

                        break;
                    }

                    hasRadixPoint = true;

                } else if (!isDigit(*buffer)) {
                    break;
                }

                length++;

            } while (*buffer != '\0');

            if (length >= CLUE_MAX_NUMERIC_LENGTH) {
                // @REPORT 4
            }

        } else if ((*buffer == '"') || (*buffer == '\'')) {
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

        } else {
            tt = (TokenTypeEnum) *buffer;

            switch (*buffer) {
                // invalid or unimplemented single-chars
                default:
                    Reporter::report(E_INVALID_CHARACTER, null, filename, line, column);
                    break;

                case '\n':
                    isSingleLineComment = false;
                    column = 1;
                    line++;
                    buffer++;
                    continue;

                case '\t':
                    column += 4;
                    buffer++;
                    continue;

                case ' ':
                    column++;
                    buffer++;
                    continue;

                case '@':
                case '$':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case ';':
                case ',':
                case '.':
                case '?':
                    break;

                case '\\': // @TODO?
                    break;

                case '#': // @TODO
                    break;

                case '`':
                    isSingleLineComment = true;
                    break;

                case '>':
                case '<':
                case '*':
                    if (*(buffer + 1) == *buffer) {
                        switch (*buffer) {
                            case '>':
                            case '<':
                            case '*':
                                length = 2;

                                // check for assignment by right/left shift and exponentiation
                                if (*(buffer + 2) == '=') {
                                    length = 3;

                                    switch (*buffer) {
                                        case '>': tt = TT_RIGHT_SHIFT_EQUALS; break;
                                        case '<': tt = TT_LEFT_SHIFT_EQUALS; break;
                                        case '*': tt = TT_EXPONENTIATION_EQUALS; break;
                                    }
                                } else {
                                    switch (*buffer) {
                                        case '>': tt = TT_RIGHT_SHIFT; break;
                                        case '<': tt = TT_LEFT_SHIFT; break;
                                        case '*': tt = TT_EXPONENTIATION; break;
                                    }
                                }
                        }
                    }

                    break;

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
            }

            buffer += length;
        }

        if (isSingleLineComment) {
            continue;
        }

        // if we're here, we should be able to make a token
        token = (Token*) pMalloc(sizeof (Token));

        token->filename = filename;
        token->line     = line;
        token->column   = column;
        token->length   = length;
        token->tt       = tt;
        token->tk       = read(buffer - length, length);
        token->bad      = bad;

        // check if we used a symbol that is a reserved operator/word
        if (token->tt == TT_SYMBOL) {
            auto entry = keywords->lookup(token->tk, token->length);

            if (entry) {
                // retroactively fix its type
                token->tt = entry->value->tt;

            } else if (false) { // @TODO global symbol table/constant lookup

            }
        }

        // being here means we have a fully-formed token, and it should probably not be modified past this point save
        // for exceptional circumstances
        Lexer::tokens->push(token);

        // handle import statement
        if (prevTokenImport) {
            if ((token->tt == TT_STRING) && (!token->bad)) {
                const char* importFilePath = trimQuotes(token->tk, token->length); // @TODO handle failure here

                // check if we've already imported the file - you shouldn't ever need to import something multiple times
                auto entry = files->lookup(importFilePath, token->length - 2);

                if (entry) { // @TODO would be cool if we could detect a recursive import vs. a duplicate import
                    Reporter::report(W_DUPLICATE_IMPORT, null, filename, line, column);

                } else {
                    files->insert(importFilePath, token->length - 2, null);

                    prevTokenImport = false; // this is necessary to stop the subsequent recursive calls from trying to import the first token
                    tokenize(clueFileRead(importFilePath), importFilePath);
                }
            } else {
                Reporter::report(E_BAD_IMPORT, null, filename, line, column);
            }
        }

        prevTokenImport = token->tt == TT_IMPORT;

        // do this only after handling EVERYTHING having to do with the token we just lexed
        column += token->length;
    }

    return Lexer::tokens;
}

