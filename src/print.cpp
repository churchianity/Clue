
#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h> // stderr, stdout, stdin? | vfprintf

#include "node.h"
#include "table.hpp"
#include "token.h"
#include "array.hpp"
#include "print.h"
#include "util.h"


// Text Colors
const char* ANSI_BLACK  = "\x001B[30m";
const char* ANSI_RED    = "\x001B[31m";
const char* ANSI_GREEN  = "\x001B[32m";
const char* ANSI_YELLOW = "\x001B[33m";
const char* ANSI_BLUE   = "\x001B[34m";
const char* ANSI_PURPLE = "\x001B[35m";
const char* ANSI_CYAN   = "\x001B[36m";
const char* ANSI_WHITE  = "\x001B[37m";

// Background Colors
const char* ANSI_BLACK_BACKGROUND  = "\x001B[40m";
const char* ANSI_RED_BACKGROUND    = "\x001B[41m";
const char* ANSI_GREEN_BACKGROUND  = "\x001B[42m";
const char* ANSI_YELLOW_BACKGROUND = "\x001B[43m";
const char* ANSI_BLUE_BACKGROUND   = "\x001B[44m";
const char* ANSI_PURPLE_BACKGROUND = "\x001B[45m";
const char* ANSI_CYAN_BACKGROUND   = "\x001B[46m";
const char* ANSI_WHITE_BACKGROUND  = "\x001B[47m";

// Wrap Text
const char* ANSI_WRAP = "\x001B[7m";

// Make text blink
const char* ANSI_BLINK = "\x001B[5m";

// Clear Console & Reset Cursor Position
const char* ANSI_CLEAR = "\x001B[2J\x001B[;H";

// Reset Colors
const char* ANSI_RESET = "\x001B[0m";


/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point with this
 */
void print(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);

    va_end(args);
}

/**
 * This should end the program, use for fatal internal errors.
 */
void die(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
    exit(1);
}

void prints(const char* s)  { print("%s\n", s); }
void print(bool b)          { print("%s\n", boolToString(b)); }
void print(char c)          { print("%c\n", c); }
void print(signed int i)    { print("%d\n", i); }
void print(unsigned int i)  { print("%u\n", i); }
void print(float f)         { print("%.14g\n", f); }
void print(double d)        { print("%.14g\n", d); }
void print(void* p)         { print("%p\n", p); }

void print(Value v) {
    if (v.type == VT_NUMBER) {
        print(v.number);
    } else {
        print(v.string);
    }
}

void print(Token* token) {
    if (!token) {
        print("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);

    print("&Token %p | file: %s, line: %u, col: %u, len: %u | tt: %s, bad?: %u | tk: %s%s%s\n"
          , (void*) token, token->filename, token->line, token->column, token->length, tt, token->bad, ANSI_YELLOW, token->tk, ANSI_RESET);
}

void print(Symbol* symbol) {
    if (!symbol) {
        print("symbol is null\n"); return;
    }

    print("&Symbol %p | name: %s, reserved?: %d\n", (void*) symbol, symbol->name, symbol->reserved);
}

void print(ASTNode* node) {
    if (!node) {
        print("node is null\n"); return;
    }

    print("&ASTNode %p | childrenCount: %u, maxChildrenCount: %u | tk: %s%s%s\n"
          , (void*) node, node->childrenCount, node->maxChildrenCount, ANSI_YELLOW, node->token->tk, ANSI_RESET);

    print("unary?: %u, postfix?: %u, call?: %u, punc?: %u\n"
         , node->unary, node->postfix, node->call, node->punctuator);

    if (node->maxChildrenCount == 0) {
        print("\n");
        // return;
    }

    for (u32 i = 0; i < node->childrenCount; i++) {
        print("\tChild %u pointer: %p | tk: %s%s%s\n"
              , i, (void*) (node->children + i), ANSI_YELLOW, (node->children + i)->token->tk, ANSI_RESET);
    }

    print("\n");
}

template <class K, class V>
void print(TableEntry<K, V>* entry) {
    print(entry->key); print(entry->value);
}

