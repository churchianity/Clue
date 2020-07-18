
#ifndef LEXER_H
#define LEXER_H


namespace Lexer {
    extern Table* files;

    extern u32 tokenCount; // how many tokens are stored in the array currently
    extern u32 capacity;   // how big is the buffer?

    extern Token* token;   // the complete token most recently lexed
    extern Token* tokens;  // every token lexer so far, in order, probably but not necessarily including the token above

    extern void tokenize(char* buffer, const char* filename);
    extern void add(Token* token);
    extern void print();
};

#endif

