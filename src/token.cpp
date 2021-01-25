
#include "print.h"
#include "token.h"

void print(Token* token) {
    print("&Token %p | r: %3d, q: %3d, l: %2d, tt: %3d | flags: %d | %s | tk: %s\n",
          (void*) token, token->line, token->column, token->length, token->tt, token->flags, token->filename, token->tk);
}

