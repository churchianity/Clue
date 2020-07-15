
#ifndef LEXER_H
#define LEXER_H

#include "table.h"
#include "token.h"
#include "util.h"


typedef struct {
    Table* files = newTable(10);

    u32 tokenCount = 0;
    u32 capacity = CLUE_INITIAL_TOKEN_ARRAY_CAPACITY;

    Token* token = NULL;
    Token* tokens = (Token*) pMalloc(sizeof (Token*) * CLUE_INITIAL_TOKEN_ARRAY_CAPACITY);

} Lexer;

extern Lexer* lexer;

void tokenize(char* buffer, const char* filename);

#endif

