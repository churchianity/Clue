
#ifndef LEXER_H
#define LEXER_H

#include "array.hpp"
#include "file.h"
#include "table.hpp"
#include "token.h"


namespace Lexer {
    extern Array<Token>* tokens;
    extern Table<const char, FileInfo>* files;

    extern void add(Token* token);
    extern void print();
    extern void clear();

    extern Array<Token>* tokenize(char* buffer, const char* filename, u32 _line = 1);
}

#endif

