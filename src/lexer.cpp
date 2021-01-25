

#include "array.hpp"
#include "lexer.h"
#include "table.hpp"
#include "token.h"
#include "types.h"


auto Lexer::tokens = new Array<Token>();
auto Lexer::files = new Table<const char, void>();

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

    return t;
}

void Lexer :: print() {
    for (u32 i = 0; i < Lexer :: tokens->length; i++) {
        print(Lexer :: tokens->data[i]);
    }
}

Array<Token>* Lexer :: tokenize(char* buffer, const char* filename, u32 _line) {
    static auto keywords = initKeywordTable();

    Token* token = null;

    u32 line = _line;
    u32 column = 1;
    u32 length = 0;

    TokenTypeEnum tt = TT_ANY;
    u8 flags = 0;

    while (*buffer != '\0') {
        length = 1;
        flags = 0;

        if (isAlpha(*buffer)) {
            tt = TT_SYMBOL;

            do {
                buffer++;

                if (!(isAlpha(*buffer) || isDigit(*buffer) || (*buffer == '_'))) {
                    break;
                }

                length++;
            } while (*buffer != '\0');

        } else if (isDigit(*buffer)) {
            tt = TT_NUMERIC;

            if (*buffer == '0') {
                switch (*(buffer + 1)) {
                    // octal constant.
                    case 'o': case 'O':
                        length++;
                        buffer++;

                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                        flags |= TF_OCTAL;

                        do {
                            buffer++;

                            if (!isOctalDigit(*buffer)) break;

                            length++;

                        } while (*buffer != '\0');
                    } break;

                    case 'x': case 'X': {
                        flags |= TF_HEXADECIMAL;

                        length++;
                        buffer++;

                        do {
                            buffer++;

                            if (!isHexDigit(*buffer)) break;

                            length++;

                        } while (*buffer != '\0');
                    } break;

                    case 'b': case 'B': {
                        flags |= TF_BINARY;

                        length++;
                        buffer++;

                        do {
                            buffer++;

                            if (!isBinaryDigit(*buffer)) break;

                            length++;

                        } while (*buffer != '\0');
                    } break;

                    default: goto normal_decimal;
                }
            } else {
normal_decimal:
                bool hasRadixPoint = false;

                do {
                    buffer++;

                    if (*buffer == '.') {
                        if (hasRadixPoint) {
                            die("number with two decimal points?\n");
                        }

                        hasRadixPoint = true;

                    } else if (!isDigit(*buffer)) {
                        break;
                    }

                    length++;

                } while (*buffer != '\0');
            }
        } else if (*buffer == '\'' || *buffer == '"') {
            tt = TT_STRING;

            char quotemark = *buffer;
            do {
                buffer++;

                if (*buffer == quotemark) {
                    buffer++;
                    length++;
                    break;
                }

                if (*buffer == '\\') {

                } else if (*buffer == '\n') {
                    column = 1;
                    line++;
                    continue;

                } else if (*buffer == '\t') {
                    column += 4;
                    continue;
                }

                length++;
            } while (*buffer != '\0');

        } else {
            tt = (TokenTypeEnum) *buffer;

            switch (*buffer) {
                case '\r':
                default: // invalid single-chars, probably weird whitespace/non-ascii
                    die("invalid char %c", *buffer);
                    break;

                case '\n':
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
                case '?':
                    break;

                case '\\':
                    break;

                case '`': {
                    buffer++;
                    column++;

                    if (*buffer == '`') {
                        do {
                            buffer++;

                            if (*buffer == '\n') {
                                column = 1;
                                line++;

                            } else if (*buffer == '`' && *(buffer + 1) == '`') {
                                column += 2;
                                buffer += 2;
                                break;
                            } else {
                                column++;
                            }
                        } while (*buffer != '\0');
                    } else {
                        while (*buffer != '\0') {
                            if (*buffer == '\n') {
                                break;
                            }

                            buffer++;
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
                    if (*(buffer + 1) == *buffer) {
                        length = 2;

                        if (*(buffer + 2) == '=') {
                            length = 3;

                            switch (*buffer) {
                                case '*': tt = TT_EXPONENTIATION_EQUALS; break;
                                case '>': tt = TT_RIGHT_SHIFT_EQUALS; break;
                                case '<': tt = TT_LEFT_SHIFT_EQUALS; break;
                                case '^': tt = TT_LOGICAL_XOR_EQUALS; break;
                                case '&': tt = TT_LOGICAL_AND_EQUALS; break;
                                case '|': tt = TT_LOGICAL_OR_EQUALS; break;
                            }
                        } else {
                            switch (*buffer) {
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
                    if (*(buffer + 1) == *buffer) {
                        switch (*buffer) {
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
                    if (*(buffer + 1) == '=') {
                        switch (*buffer) {
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

            buffer += length;
        }

        token = (Token*) pMalloc(sizeof (Token));

        token->filename = filename;
        token->line     = line;
        token->column   = column;
        token->length   = length;
        token->tt       = tt;
        token->tk       = read(buffer - length, length);
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




