
#ifndef LEXER_H
#define LEXER_H

#include "clue.h"
#include "token.h"

Token* tokenize(char* buffer, const char* filename);

#endif

