
#include <execinfo.h> // backtrace, backtrace_symbols
#include <stdarg.h> // va_list, va_start, va_end, ...
#include <stdio.h> // FILE, stderr, stdout, stdin? | vfprintf
#include <stdlib.h> // exit

#include "alloc.h"
#include "print.h"
#include "token.h"
#include "types.h"
#include "value.h"


/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point with this
 */
void print(const char* format, ...) {
    if (!format) {
        print("null\n"); return;
    }

    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);

    va_end(args);
}

/**
 * Prints a stack trace.
 * maxFrames = 63 is arbitrary
 */
void trace() {
    #define MAX_FRAMES__ 63
    void** stack = (void**) pMalloc(sizeof (void*) * MAX_FRAMES__);
    u32 stackSize = backtrace(stack, MAX_FRAMES__);

    // resolve addresses into strings containing "filename(function+address)"
    // this array must be free()-ed
    char** traces = backtrace_symbols(stack, stackSize);

    if (stackSize < 2) {
        fprintf(stderr, "stack has a weird number (%d) of frames!\n", stackSize);
        exit(1);
    }

    // iterate over the returned symbol lines. skip the first, it is the address of this function
    for (u32 i = 1; i < stackSize; i++) {
        fprintf(stderr, "  %s\n", traces[i]);
    }

    pFree(traces);
    pFree(stack);
}

/**
 * This should end the program, use for fatal **internal only** errors.
 */
void die(const char* format, ...) {
    trace();

    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
    exit(1);
}

void print(bool b)         { print("%s\n", boolToString(b)); }
void print(char c)         { print("%c\n", c); }
void print(signed int i)   { print("%d\n", i); }
void print(unsigned int i) { print("%u\n", i); }
void print(float f)        { print("%.14g\n", f); }
void print(double d)       { print("%.14g\n", d); }
void print(void* p)        { print("%p\n", p); }

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

    print("&Token %p | file: %s, line: %u, col: %u, len: %u | tt: %s, flags: %u | tk: %s%s%s\n"
          , (void*) token, token->filename, token->line, token->column, token->length, tt, token->flags, ANSI_YELLOW, token->tk, ANSI_RESET);
}

void print(ASTNode* node) {
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

void print(Program* program) {
    if (!program) {
        print("program is null\n"); return;
    }

    print("&Program %p | &statements: %p, #statements: %u\n", (void*) program, (void*) program->statements, program->statements->length);

    program->statements->forEach(
        [] (ASTNode* root) {
            traverse(root,
                [] (ASTNode* node) {
                    // only print nodes with children (the node will print some info about its children)
                    if (node->children) {
                        print(node);
                    }
                }
            );
        }
    );
}

