
#include "message.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "stack.h"
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


void print(const Token* token) {
    if (!token) {
        printf("token is NULL\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);
    const char* bad = boolToString(token->bad);

    printf("%p | file: %s, line: %u, col: %u, len: %u | tt: %s, bad: %s, tk: %s\n"
           , (void*) token, token->filename, token->line, token->column, token->length, tt, bad, token->tk);
}

void print(const ASTNode* node) {
    if (!node) {
        printf("node is NULL\n"); return;
    }

    printf("%p | childrenCount: %u, maxChildrenCount: %u\nunary: %d | postfix: %d | call: %d | precedence: %d\n"
           , (void*) node, node->childrenCount, node->maxChildrenCount, node->unary, node->postfix, node->call, node->precedence);

    printf("Token: "); print(node->token);

    for (u32 i = 0; i < node->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (node->children + i));
    }

    printf("\n");
}

void print(const Table* table) {
    if (!table) {
        printf("table is NULL\n"); return;
    }

    printf("%p | capacity: %u | entries:\n", (void*) table, table->capacity);

    for (u32 i = 0; i < table->capacity; ++i) {
        TableEntry* entry = *(table->entries + i);
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

void print(const Stack* stack) {
    if (!stack) {
        printf("stack is NULL\n"); return;
    }

    const char* grow = boolToString(stack->grow);

    printf("capacity: %u, grow?: %s, top: %u, size: %u, data: %p\n"
            , stack->capacity
            , grow
            , stack->top
            , stack->size(stack)
            , *stack->data);
}

void print(const Message* message) {
    //printf("%s%s%s: "
    //       , messageSeverityToColor(message->severity)
    //       , messageSeverityToString(message->severity), ANSI_RESET



      //     );
}

