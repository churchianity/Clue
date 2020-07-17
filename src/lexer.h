
#ifndef LEXER_H
#define LEXER_H

#include "clue.h"
#include "table.h"
#include "token.h"
#include "util.h"


typedef struct {
    Table* files;

    u32 tokenCount;
    u32 capacity;

    Token* token;
    Token* tokens;

} Lexer;

extern Lexer* lexer;

void initLexer();
void tokenize(char* buffer, const char* filename);

#endif

