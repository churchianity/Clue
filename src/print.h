
#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h> // stderr, stdout, stdin? | vfprintf

#include "runtime.h" // Value*
#include "node.h" // ASTNode*
#include "token.h" // Token*
#include "trace.h"


// Text Colors
#define ANSI_BLACK  "\x001B[30m"
#define ANSI_RED    "\x001B[31m"
#define ANSI_GREEN  "\x001B[32m"
#define ANSI_YELLOW "\x001B[33m"
#define ANSI_BLUE   "\x001B[34m"
#define ANSI_PURPLE "\x001B[35m"
#define ANSI_CYAN   "\x001B[36m"
#define ANSI_WHITE  "\x001B[37m"

// Background Colors
#define ANSI_BLACK_BACKGROUND  "\x001B[40m"
#define ANSI_RED_BACKGROUND    "\x001B[41m"
#define ANSI_GREEN_BACKGROUND  "\x001B[42m"
#define ANSI_YELLOW_BACKGROUND "\x001B[43m"
#define ANSI_BLUE_BACKGROUND   "\x001B[44m"
#define ANSI_PURPLE_BACKGROUND "\x001B[45m"
#define ANSI_CYAN_BACKGROUND   "\x001B[46m"
#define ANSI_WHITE_BACKGROUND  "\x001B[47m"

// Wrap Text
#define ANSI_WRAP "\x001B[7m"

// Make text blink
#define ANSI_BLINK "\x001B[5m"

// Clear Console & Reset Cursor Position
#define ANSI_CLEAR "\x001B[2J\x001B[;H"

// Reset Colors
#define ANSI_RESET "\x001B[0m"

/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point with this
 */
inline void print(const char* format, ...) {
    if (!format) {
        print("null\n"); return;
    }

    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);

    va_end(args);
}

/**
 * This should end the program, use for fatal **internal only** errors.
 */
inline void die(const char* format, ...) {
    trace();

    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
    exit(1);
}

inline void print(bool b)         { print("%s\n", boolToString(b)); }
inline void print(char c)         { print("%c\n", c); }
inline void print(signed int i)   { print("%d\n", i); }
inline void print(unsigned int i) { print("%u\n", i); }
inline void print(float f)        { print("%.14g\n", f); }
inline void print(double d)       { print("%.14g\n", d); }
inline void print(void* p)        { print("%p\n", p); }

inline void print(Value v) {
    if (v.type == VT_NUMBER) {
        print(v.number);

    } else {
        print(v.string);
    }
}

inline void print(Token* token) {
    if (!token) {
        print("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);

    print("&Token %p | file: %s, line: %u, col: %u, len: %u | tt: %s, flags: %u | tk: %s%s%s\n"
          , (void*) token, token->filename, token->line, token->column, token->length, tt, token->flags, ANSI_YELLOW, token->tk, ANSI_RESET);
}

/*
template <class T>
inline void print(Array<T>* array) {
    print("[");
    for (u32 i = 0; i < array->length; i++) {
        print(array->data[i]);
    }
    print("]\n");
}
*/

inline void print(ASTNode* node) {
    if (!node) {
        print("node is null\n"); return;
    }

    print("&ASTNode %p", (void*) node);

    if (node->children) {
        print(" | childrenCount: %u", node->children->length);
    }

    print(" | tk: %s%s%s\n", ANSI_YELLOW, node->token->tk, ANSI_RESET);

    if (node->children) {
        for (u32 i = 0; i < node->children->length; i++) {
            print("    Child #%u : &ASTNode %p | tk: %s%s%s\n"
                  , i, (void*) node->children->data[i], ANSI_YELLOW, node->children->data[i]->token->tk, ANSI_RESET);
        }

        print("\n");
    }
}

inline void print(Program* program) {
    if (!program) {
        print("program is null\n"); return;
    }

    print("&Program %p | &statements: %p, #statements: %u\n", (void*) program, (void*) program->statements, program->statements->length);

    program->statements->forEach(
        [] (ASTNode* root) {
            traverse(root,
                [] (ASTNode* node) {
                    if (node->children) {
                        print(node);
                    }
                }
            );
        }
    );
}

#endif

