

#include "array.hpp"
#include "lexer.h"
#include "table.hpp"
#include "reporter.h"
#include "message.h"
#include "token.h"
#include "types.h"


Array<Token>* Lexer::tokens = new Array<Token>();
Table<const char, void>* Lexer::files = new Table<const char, void>();

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

    t->insert("import",         6, keyword(TT_IMPORT));
    t->insert("if",             2, keyword(TT_IF));
    t->insert("else",           4, keyword(TT_ELSE));
    t->insert("do",             2, keyword(TT_DO));
    t->insert("while",          5, keyword(TT_WHILE));
    t->insert("return",         6, keyword(TT_RETURN));
    t->insert("and",            3, keyword(TT_AND));
    t->insert("or",             2, keyword(TT_OR));
    t->insert("not",            3, keyword(TT_NOT));
    t->insert("elseif",         6, keyword(TT_ELSEIF));

    return t;
}

void Lexer :: print() {
    for (u32 i = 0; i < Lexer :: tokens->length; i++) {
        print(Lexer :: tokens->data[i]);
    }
}

/**
 * Given a null-terminated string |buffer|, append to the lexer's tokens array.
 *
 * |_line| is sometimes necessary to provide, because you can optionally lex tokens in line-buffered batches.
 * The typical case of this happening is typing code in line-by-line in sandbox mode.
 * In most cases you won't provide it though, and it defaults to 1.
 */
Array<Token>* Lexer :: tokenize(char* buffer, const char* filename, u32 _line) {
    static auto keywords = initKeywordTable();

    Token* token = null;

    u32 line = _line;
    u32 column = 1;
    u32 length = 0;

    TokenTypeEnum tt = TT_ANY;
    u8 flags = 0;

    char* cursor = buffer;
    while (*cursor != '\0') {
        length = 1;
        flags = 0;

        if (Str :: isAlpha(*cursor)) {
            tt = TT_SYMBOL;

            do {
                cursor++;

                if (!(Str :: isAlpha(*cursor) || Str :: isDigit(*cursor) || (*cursor == '_'))) {
                    break;
                }

                length++;

            } while (*cursor != '\0');

        } else if (Str :: isDigit(*cursor)) {
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

                            if (!Str :: isOctalDigit(*cursor)) break;

                            length++;

                        } while (*cursor != '\0');
                    } break;

                    case 'x': case 'X': {
                        flags |= TF_HEXADECIMAL;

                        length++;
                        cursor++;

                        do {
                            cursor++;

                            if (!Str :: isHexDigit(*cursor)) break;

                            length++;

                        } while (*cursor != '\0');
                    } break;

                    case 'b': case 'B': {
                        flags |= TF_BINARY;

                        length++;
                        cursor++;

                        do {
                            cursor++;

                            if (!Str :: isBinaryDigit(*cursor)) break;

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
                            // this is maybe the best example of why a 'maybeBad' flag might be worthwhile, so if the parser finds out later that some expression
                            // '2..3' makes no sense, it can guess why - a typo of '2.3'
                            cursor--;
                            break;
                        }

                        hasRadixPoint = true;

                    } else if (!Str :: isDigit(*cursor)) {
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
            // the best example of this is '&&' vs 'and'. We use 'and'. But it's probably a common enough case someone forgets, since we otherwise
            // have C-like syntax, and they type '&&'. So we have a lot more options and power of what to do in case of a typo here.
            tt = (TokenTypeEnum) *cursor;

            switch (*cursor) {
                case '\r':
                    // @TODO depending on compiler flag, report.
                    continue;

                case '_':
                    // leading underscores aren't supported by the language
                    // after the first character in a symbol is fine though.
                    // @TODO check if there's a symbol after it so the reported message is accurate
                    // .. it could just be a typo
                    die("leading underscores aren't allowed in user-defined symbols, like variable or function names.\n");
                    continue;

                default: // invalid single-chars, probably weird whitespace/non-ascii
                    // @TODO do some work to find out what exactly caused the problem. this will probably most of the time be a paste-error
                    // - we can check if it's a Unicode space separator or other common case and report better.
                    Reporter :: report(E_WEIRD_CODEPOINT, null, filename, line, column, (u32) *cursor);
                    break;

                case '\n':
                    column = 1;
                    line++;
                    cursor++;
                    continue;

                case '\t':
                    column += 4;
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
                case '?':
                    break;

                case '\\':
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
                                case '^': tt = TT_LOGICAL_XOR_EQUALS; break;
                                case '&': tt = TT_LOGICAL_AND_EQUALS; break;
                                case '|': tt = TT_LOGICAL_OR_EQUALS; break;
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
                            case '+': tt = TT_INCREMENT;      break;
                            case '-': tt = TT_DECREMENT;      break;
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
            }

            cursor += length;
        }

        token = (Token*) pMalloc(sizeof (Token));

        token->filename = filename;
        token->line     = line;
        token->column   = column;
        token->length   = length;
        token->tt       = tt;
        token->tk       = Str :: read(cursor - length, length);
        token->flags    = flags;

        if (token->tt == TT_SYMBOL) {
            const auto entry = keywords->lookup(token->tk, token->length);

            if (entry) token->tt = entry->value->tt;
        }

        Lexer :: tokens->push(token);

        column += token->length;
    }

    return Lexer::tokens;
}




