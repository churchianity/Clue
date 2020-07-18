
#ifndef LEXER_H
#define LEXER_H

#include "clue.h"
#include "table.h"
#include "token.h"
#include "util.h"


typedef struct {
    Table* files;

    u32 tokenCount; // how many tokens are stored in the array currently
    u32 capacity;   // how big is the buffer?

    Token* token;   // the complete token most recently lexed
    Token* tokens;  // every token lexer so far, in order, probably but not necessarily including the token above

} Lexer;

extern Lexer* lexer;

void initLexer();
void tokenize(char* buffer, const char* filename);

#endif

