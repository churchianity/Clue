
#ifndef LEXER_H
#define LEXER_H

#include "clue.h"
#include "token.h"


Token* lexSymbol(char* buffer, const char* filename, u32 line, u32 column);
Token* lexNumeric(char* buffer, const char* filename, u32 line, u32 column);
Token* lexString(char* buffer, const char* filename, u32 line, u32 column);
Token* lexOperator(char* buffer, const char* filename, u32 line, u32 column);

Token* tokenize(char* buffer, const char* filename);

#endif

