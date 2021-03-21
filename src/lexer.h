
#ifndef LEXER_H
#define LEXER_H

#include "array.hpp"
#include "table.hpp"
#include "token.h"
#include "types.h"

extern Array<Token>* Lexer_tokens;
extern Table<const char, void>* Lexer_files;
extern void Lexer_print();

extern Array<Token>* Lexer_tokenize(char* buffer, const char* filename, u32 _line = 1);

#endif

