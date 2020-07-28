
#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h> // stderr, stdout, stdin? | fprintf, printf

#include "message.h"
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
void die(const char* formatString...) {
    va_list args;
    va_start(args, formatString);

    fprintf(stderr, formatString, args);

    va_end(args);
    exit(1);
}

void print(const char* string) {
    printf("%s", string);
}

/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point
 */
void print(const char* formatString...) {
    va_list args;
    va_start(args, formatString);

    printf(formatString, args);

    va_end(args);
}

void print(const Token* token) {
    if (!token) {
        printf("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);

    printf("&Token %p | file: %s, line: %u, col: %u, len: %u | tt: %s, bad?: %u | tk: %s%s%s\n"
           , (void*) token, token->filename, token->line, token->column, token->length, tt, token->bad, ANSI_YELLOW, token->tk, ANSI_RESET);
}

void print(const Operator* op) {
    if (!op) {
        printf("operator is null\n"); return;
    }

    printf("&Operator %p | name: %s | prec: %u, assoc: %d, unary?: %u, postfix?: %u, call?: %u\n"
            , (void*) op, op->name, op->precedence, op->associativity, op->unary, op->postfix, op->call);
}

void print(const Symbol* symbol) {
    if (!symbol) {
        printf("symbol is null\n"); return;
    }

    printf("&Symbol %p | name: %s, reserved?: %d\n", (void*) symbol, symbol->name, symbol->reserved);
}

void print(const ASTNode* node) {
    if (!node) {
        printf("node is null\n"); return;
    }

    printf("&ASTNode %p\n", (void*) node);
    print(node->token);
}

void print(const ASTSymbolNode* node) {
    if (!node) {
        printf("node is null\n"); return;
    }

    printf("&ASTNode %p\n", (void*) node);
    print(node->symbol);
}

void print(const ASTOperatorNode* node) {
    if (!node) {
        printf("node is null\n"); return;
    }

    printf("&ASTNode %p | childrenCount: %u, maxChildrenCount: %u\n"
           , (void*) node, node->childrenCount, node->maxChildrenCount);

    print(node->op);

    for (u32 i = 0; i < node->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (node->children + i));
    }

    printf("\n");
}

template <class K, class V>
void print(const Table<K, V>* table) {
    if (!table) {
        printf("table is null\n"); return;
    }

    printf("&Table %p | capacity: %u | entries:\n", (void*) table, table->capacity);

    for (u32 i = 0; i < table->capacity; ++i) {
        TableEntry<K, V>* entry = *(table->entries + i);
        printf("%u : %p", i, (void*) entry);

        if (entry) {
            printf(" | %s", entry->key);

            while (entry->next) {
                entry = entry->next;

                printf(" ---> %p | %s", (void*) entry, entry->key);
            }
        }

        printf("\n");
    }
}

template <class T>
void print(const Stack<T>* stack) {
    if (!stack) {
        printf("stack is null\n"); return;
    }

    printf("&Stack %p | capacity: %u, grow?: %u, top: %u, size: %u, data:\n"
            , (void*) stack
            , stack->capacity
            , stack->grow
            , stack->top
            , stack->size());

    for (u32 i = 0; i < stack->size(); i++) {
        print(stack->data[i]);
    }
}

static inline char* makePointyThing(u32 column) {
    char* buffer = (char*) pCalloc(column + 1, sizeof (char));

    for (u32 i = 0; i < (column - 1); i++) {
        buffer[i] = ' ';
    }

    buffer[column - 1] = '^';
    buffer[column] = '\0';

    return buffer;
}

/**
 *    lint: alphabetical characters can't follow digits in identifier names
 *    in function 'funcName': ./baz.clue:124:10
 *            Int x2n;
 *                  ^
 *
 *    warn: unused variable 'x'
 *    in function 'doSomeStuff': ./bar.clue:10:10
 *            Int x;
 *                ^
 *
 *    error: missing right-hand operand for operator
 *    in function 'funcName': ./../src/foo.clue:14:51
 *            Int x := 4 !;
 *                       ^
 *
 *    error: missing right-hand operand for operator
 *    ./../src/foo.clue:14:51
 *            Int x := 4 !;
 *                       ^
 */
void print(const Message* message) {
    const char* fn = message->functionName;
    char* pointyThing = makePointyThing(message->column);

    // i'm so sorry.
    printf("\n    %s%s%s: %s\n    %s%s%s%s:%u:%u\n    %s\n    %s%s%s\n"
           , messageSeverityToColor(message->severity), messageSeverityToString(message->severity), ANSI_RESET
           , message->content
           , fn ? "in function '" : "", fn ? fn : "", fn ? "': " : ""
           , message->filename, message->line, message->column
           , reconstruct(message->line)
           , ANSI_RED, pointyThing, ANSI_RESET
    );

    free(pointyThing);
}

