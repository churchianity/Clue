
#include "message.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "stack.h"
#include "print.h"
#include "util.h"


void print(const Token* token) {
    if (!token) {
        printf("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);
    const char* bad = boolToString(token->bad);

    printf("%p | file: %s, line: %u, col: %u, len: %u | tt: %s, bad: %s | "
           , (void*) token, token->filename, token->line, token->column, token->length, tt, bad);

    switch (token->tt) {
        case TT_SYMBOL: printf("symbol: %s\n", token->symbol->name); break;
        case TT_NUMERIC: printf("number: %.2f\n", token->number); break;
        case TT_STRING: printf("string: %s\n", token->string); break;

        default: // TT_OPERATORS
            printf("operator: %s\n", token->op->name);
            break;
    }
}

void print(const ASTNode* node) {
    if (!node) {
        printf("branch node is null\n"); return;
    }

    printf("%p | childrenCount: %u, maxChildrenCount: %u\n"
           , (void*) node, node->childrenCount, node->maxChildrenCount);

    printf("Token: "); print(node->token);

    for (u32 i = 0; i < node->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (node->children + i));
    }

    printf("\n");
}

void print(const Table* table) {
    if (!table) {
        printf("table is null\n"); return;
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
        printf("stack is null\n"); return;
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

