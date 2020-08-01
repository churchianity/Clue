
#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h> // stderr, stdout, stdin? | fprintf, printf

#include "node.h"
#include "table.hpp"
#include "token.h"
#include "stack.hpp"
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
 * This should end the program, use for fatal internal errors.
 */
void die(const char* format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, format, args);

    va_end(args);
    exit(1);
}

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

void print(const Token* token) {
    if (!token) {
        print("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);

    print("&Token %p | file: %s, line: %u, col: %u, len: %u | tt: %s, bad?: %u | tk: %s%s%s\n"
          , (void*) token, token->filename, token->line, token->column, token->length, tt, token->bad, ANSI_YELLOW, token->tk, ANSI_RESET);
}

void print(const Symbol* symbol) {
    if (!symbol) {
        print("symbol is null\n"); return;
    }

    print("&Symbol %p | name: %s, reserved?: %d\n", (void*) symbol, symbol->name, symbol->reserved);
}

void print(const ASTNode* node) {
    if (!node) {
        print("node is null\n"); return;
    }

    print("&ASTNode %p | childrenCount: %u, maxChildrenCount: %u | tk: %s%s%s\n"
          , (void*) node, node->childrenCount, node->maxChildrenCount, ANSI_YELLOW, node->token->tk, ANSI_RESET);

    if (!node->childrenCount) {
        print("\n");
        return;
    }

    printf("prec: %u, assoc: %d, unary?: %u, postfix?: %u, call?: %u\n"
           , node->precedence, node->associativity, node->unary, node->postfix, node->call);

    for (u32 i = 0; i < node->childrenCount; i++) {
        print("\tChild %u pointer: %p | tk: %s%s%s\n"
              , i, (void*) (node->children + i), ANSI_YELLOW, (node->children + i)->token->tk, ANSI_RESET);
    }

    print("\n");
}

template <class K, class V>
void print(const Table<K, V>* table) {
    if (!table) {
        print("table is null\n"); return;
    }

    print("&Table %p | capacity: %u | entries:\n", (void*) table, table->capacity);

    for (u32 i = 0; i < table->capacity; ++i) {
        TableEntry<K, V>* entry = *(table->entries + i);
        print("%u : %p", i, (void*) entry);

        if (entry) {
            print(" | %s", entry->key);

            while (entry->next) {
                entry = entry->next;

                print(" ---> %p | %s", (void*) entry, entry->key);
            }
        }

        print("\n");
    }
}

template <class T>
void print(const Stack<T>* stack) {
    if (!stack) {
        print("stack is null\n"); return;
    }

    print("&Stack %p | capacity: %u, grow?: %u, top: %u, size: %u, data:\n"
          , (void*) stack
          , stack->capacity
          , stack->grow
          , stack->top
          , stack->size());

    for (u32 i = 0; i < stack->size(); i++) {
        print(stack->data[i]);
    }
}

