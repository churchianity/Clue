
#include <stdio.h>

#include "token.h"
#include "node.h"
#include "stack.h"
#include "lexer.h"
#include "print.h"
#include "table.h"
#include "util.h"


void print(const Token* token) {
    if (!token) {
        printf("token is null\n"); return;
    }

    const char* tt = tokenTypeToString(token->tt);
    const char* bad = boolToString(token->bad);

    // show something for empty strings
    const char* tk = (strln(token->tk) == 0) ? "(empty string)" : token->tk;

    printf("%p | file: %s, line: %u, col: %u len: %u | tt: %s, bad: %s, tk: %s\n"
           , (void*) token, token->filename, token->line, token->column, token->length, tt, bad, tk);
}

void print(const ASTNode* node) {
    if (!node) {
        printf("node is null\n"); return;
    }

    printf("%p | childrenCount: %u\n", (void*) node, node->childrenCount);

    printf("Token: "); print(node->token);

    for (u32 i = 0; i < node->childrenCount; i++) {
        printf("\tChild %u pointer: %p\n", i, (void*) (node->children + i));
    }

    printf("\n");
}

void print(const Table* table) {
    if (!table) {
        printf("stack is null\n"); return;
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

void print(const Lexer* lexer) {
    printf("%p | lexer count: %u | capacity: %u\nfiles:", (void*) lexer, lexer->tokenCount, lexer->capacity);
    print(lexer->files);

    printf("\nlast token: "); print(lexer->token); printf("\ntokens:\n");

    for (u32 i = 0; i < lexer->tokenCount; i++) {
        print(lexer->tokens + i);
    }
}

