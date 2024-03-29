
#include "array.hpp"
#include "lexer.h"
#include "table.hpp"
#include "reporter.h"
#include "message.h"
#include "token.h"
#include "types.h"


Array<Token>* Lexer_tokens = new Array<Token>();
Table<const char, void>* Lexer_files = new Table<const char, void>();

struct Keyword {
    TokenTypeEnum tt;
};

static inline Keyword* keyword(TokenTypeEnum tt) {
    Keyword* kw = (Keyword*) pMalloc(sizeof (Keyword));

    kw->tt = tt;

    return kw;
}

static Table<const char, Keyword>* initKeywordTable() {
    auto t = new Table<const char, Keyword>();

    return t;
}

void Lexer_print() {
    for (u32 i = 0; i < Lexer_tokens->length; i++) {
        print(Lexer_tokens->data[i]);
    }
}

/**
 * Given a null-terminated string |buffer|, append to the lexer's tokens array.
 *
 * |_line| is sometimes necessary to provide, because you can optionally lex tokens in line-buffered batches.
 * The typical case of this happening is typing code in line-by-line in sandbox mode.
 * In most cases you won't provide it though, and it defaults to 1.
 */
Array<Token>* Lexer_tokenize(char* buffer, const char* filename, u32 _line) {
    static auto keywords = initKeywordTable();

    Token* token = null;

    u32 line = _line;
    u32 column = 1;
    u32 length = 0;

    TokenTypeEnum tt = (TokenTypeEnum) 0;
    u8 flags = 0;

    char* cursor = buffer;
    while (*cursor != '\0') {
        length = 1;
        flags = 0;

        if (Str_isAlpha(*cursor) || *cursor == '_') {
            tt = TT_SYMBOL;

            do {
                cursor++;

                if (!(Str_isAlpha(*cursor) || Str_isDigit(*cursor) || (*cursor == '_'))) {
                    break;
                }

                length++;

            } while (*cursor != '\0');

        } else if (Str_isDigit(*cursor)) {
            tt = TT_NUMERIC;

            if (*cursor == '0') {
                switch (*(cursor + 1)) {
                    // octal constant.
                    case 'o': case 'O':
                        length++;
                        cursor++;

                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                        flags |= TF_OCTAL;

                        do {
                            cursor++;

                            if (!Str_isOctalDigit(*cursor)) break;

                            length++;

                        } while (*cursor != '\0');
                    } break;

                    case 'x': case 'X': {
                        flags |= TF_HEXADECIMAL;

                        length++;
                        cursor++;

                        do {
                            cursor++;

                            if (!Str_isHexDigit(*cursor)) break;

                            length++;

                        } while (*cursor != '\0');
                    } break;

                    case 'b': case 'B': {
                        flags |= TF_BINARY;

                        length++;
                        cursor++;

                        do {
                            cursor++;

                            if (!Str_isBinaryDigit(*cursor)) break;

                            length++;

                        } while (*cursor != '\0');
                    } break;

                    default: goto normal_decimal;
                }
            } else {
normal_decimal:
                bool hasRadixPoint = false;
                do {
                    cursor++;

                    if (*cursor == '.') {
                        if (hasRadixPoint) {
                            // @NOTE there's a complication here if we want to use a '..' operator.
                            // maybe 2..3 is a typo, meaning the float 2.3, but maybe also it's meant to express the range from 2 -> 3,
                            // or a concatenation of the string-casted numbers 2 and 3.
                            // if we want to add this operator, which I think we probably do, we need to rewind the cursor one step here
                            // this is maybe the best example of why a 'maybeBad' flag might be worthwhile
                            // If the parser finds out later that some expression '2..3' makes no sense, it can guess why - a typo of '2.3'
                            cursor--;
                            break;
                        }

                        hasRadixPoint = true;

                    } else if (!Str_isDigit(*cursor)) {
                        break;
                    }

                    length++;

                } while (*cursor != '\0');
            }
        } else if (*cursor == '\'' || *cursor == '"') {
            tt = TT_STRING;

            char quotemark = *cursor;
            do {
                cursor++;

                if (*cursor == quotemark) {
                    cursor++;
                    length++;
                    break;
                }

                if (*cursor == '\\') {
                    // @TODO escape characters.

                } else if (*cursor == '\n') {
                    column = 1;
                    line++;
                    continue;

                } else if (*cursor == '\t') {
                    column += 4;
                    continue;
                }

                length++;
            } while (*cursor != '\0');

        } else {
            // operators.
            // assume the operator type is just the single char we have at the cursor, and exhaustively check if we are wrong until we can't be.
            //
            // because of the nature of the lexer, and the early stage the language is in, we are greedily parsing many, many tokens, despite
            // many of the tokens we lex not having yet a defined purpose or grammar.
            //
            // there are some cases where we are pretty sure we don't want to use an operator at all, but lex it anyway for better reporting
            // the best example of this is '&&' vs 'and'. We use 'and'.
            // It's probably a common enough case someone forgets, since we otherwise have C-like syntax.
            tt = (TokenTypeEnum) *cursor;

            switch (*cursor) {
                case '\r':
                    // @TODO depending on compiler flag, report.
                    continue;

                default: // invalid single-chars, probably weird whitespace/non-ascii
                    // @TODO do some work to find out what exactly caused the problem. this will probably most of the time be a paste-error
                    // - we can check if it's a Unicode space separator or other common case and report better.
                    Reporter_report(E_WEIRD_CODEPOINT, null, filename, line, column, (u32) *cursor);
                    break;

                case '\n':
                    column = 1;
                    line++;
                    cursor++;
                    continue;

                case '\t':
                    column += 4; // @TODO this is not a robust way to do column count. many people set their tab width to be not 4.
                    cursor++;
                    continue;

                case ' ':
                    column++;
                    cursor++;
                    continue;

                case '@':
                case '#':
                case '$':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case ';':
                case '.':
                case ',':
                    break;

                case '\\':
                    // @TODO?
                    break;

                case '`': {
                    cursor++;
                    column++;

                    if (*cursor == '`') {
                        // multi-line comment
                        do {
                            cursor++;

                            if (*cursor == '\n') {
                                column = 1;
                                line++;

                            } else if (*cursor == '`' && *(cursor + 1) == '`') {
                                column += 2;
                                cursor += 2;
                                break;

                            } else {
                                column++;
                            }
                        } while (*cursor != '\0');
                    } else {
                        // single-line comment
                        while (*cursor != '\0') {
                            if (*cursor == '\n') {
                                break;
                            }

                            cursor++;
                            column++;
                        }
                    }
                } continue;

                case '>':
                case '<':
                case '^':
                case '*':
                case '&':
                case '|':
                    if (*(cursor + 1) == *cursor) {
                        length = 2;

                        if (*(cursor + 2) == '=') {
                            length = 3;

                            switch (*cursor) {
                                case '*': tt = TT_EXPONENTIATION_EQUALS; break;
                                case '>': tt = TT_RIGHT_SHIFT_EQUALS; break;
                                case '<': tt = TT_LEFT_SHIFT_EQUALS; break;
                            }
                        } else {
                            switch (*cursor) {
                                case '*': tt = TT_EXPONENTIATION; break;
                                case '>': tt = TT_RIGHT_SHIFT; break;
                                case '<': tt = TT_LEFT_SHIFT; break;
                                case '^': tt = TT_LOGICAL_XOR; break;
                                case '&': tt = TT_LOGICAL_AND; break;
                                case '|': tt = TT_LOGICAL_OR; break;
                            }
                        }
                    }

                    break;

                case '+':
                case '-':
                case '/':
                case '=':
                    if (*(cursor + 1) == *cursor) {
                        switch (*cursor) {
                            case '+':
                                die("clue doesn't support the increment operator. use '+= 1'");
                                break;
                            case '-':
                                die("clue doesn't support the decrement operator. use '-= 1'");
                                break;

                            case '*': tt = TT_EXPONENTIATION; break;
                            case '=': tt = TT_EQUALITY;       break;
                        }

                        length = 2;
                    }

                case '!':
                case '%':
                case '~':
                case ':':
                    if (*(cursor + 1) == '=') {
                        switch (*cursor) {
                            case '!': tt = TT_NOT_EQUALS;            break;
                            case '%': tt = TT_MODULO_EQUALS;         break;
                            case '^': tt = TT_BITWISE_XOR_EQUALS;    break;
                            case '&': tt = TT_BITWISE_AND_EQUALS;    break;
                            case '|': tt = TT_BITWISE_OR_EQUALS;     break;
                            case '*': tt = TT_TIMES_EQUALS;          break;
                            case '+': tt = TT_PLUS_EQUALS;           break;
                            case '-': tt = TT_MINUS_EQUALS;          break;
                            case ':': tt = TT_COLON_EQUALS;          break;
                            case '>': tt = TT_GREATER_THAN_OR_EQUAL; break;
                            case '<': tt = TT_LESS_THAN_OR_EQUAL;    break;
                            case '/': tt = TT_DIVIDE_EQUALS;         break;
                        }

                        length = 2;
                    }

                    break;

                case '?':
                    if (*(cursor + 1) == ':') {
                        tt = TT_QUESTION_MARK_COLON;
                        length = 2;

                    } else {
                        // @TODO
                        die("question mark without a colon after.\n");
                    }

                    break;
            }

            cursor += length;
        }

        token = (Token*) pMalloc(sizeof (Token));

        token->filename = filename;
        token->line     = line;
        token->column   = column;
        token->length   = length;
        token->tt       = tt;
        token->tk       = Str_read(cursor - length, length);
        token->flags    = flags;

        if (token->tt == TT_SYMBOL) {
            const auto entry = keywords->lookup(token->tk, token->length);

            if (entry) token->tt = entry->value->tt;
        }

        Lexer_tokens->push(token);

        column += token->length;
    }

    return Lexer_tokens;
}

