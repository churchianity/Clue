

#include "array.hpp"
#include "table.hpp"
#include "token.h"
#include "types.h"

namespace Lexer {
    extern Array<Token>* tokens;
    extern Table<const char, void>* files;
    extern void reset();

    extern Array<Token>* tokenize(char* buffer, const char* filename, u32 _line = 1);
}

