

#include "array.hpp"
#include "lexer.h"
#include "table.hpp"
#include "token.h"
#include "types.h"


Array<Token>* Lexer::tokens = new Array<Token>();
Table<const char, void>* Lexer::files = new Table<const char, void>();

Array<Token>* Lexer :: tokenize(char* buffer, const char* filename, u32 _line) {
    return null;
}




