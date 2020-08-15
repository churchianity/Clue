
#ifndef LEXER_H
#define LEXER_H

#include "array.hpp"
#include "table.hpp"
#include "token.h"


namespace Lexer {
    // extern Table<const char, TokenTypeEnum>* keywords; // names of all the reserved keywords in the language

    // extern u32 tokenCount; // how many tokens are stored in the array currently
    // extern u32 capacity;   // how big is the buffer?

    // extern Token* token;   // the complete token most recently lexed
    // extern Token* tokens;  // every token lexed so far, in order, probably but not necessarily including the token above

    extern Array<Token>* tokens;

    extern void add(Token* token);
    extern void print();
    extern void clear();

    extern Array<Token>* tokenize(char* buffer, const char* filename, u32 _line = 1);
}

#endif

