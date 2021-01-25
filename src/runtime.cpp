

#include <stdio.h> // stdin

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"
#include "runtime.h"


Array<ASTNode>* program = new Array<ASTNode>();

void Runtime :: printProgramTree(Array<ASTNode>* program) {
    if (program != null) {
        for (u32 i = 0; i < program->length; i++) {
            prettyPrintTree(program->data[i], "", true);
        }
    }
}

void Runtime :: doIt(char* buffer, const char* filename) {
    Array<Token>* tokens = Lexer :: tokenize(buffer, filename);
    Array<ASTNode>* program = Parser :: parse(tokens);
    printProgramTree(program);
}

void Runtime :: interactive() {
    const u32 CLUE_SANDBOX_MODE_MAX_LINE_LENGTH = 160;
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    u32 line = 1;

    do {
        print(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            die("error reading line and storing it here: %p\n", (void*) s);
        }

        switch (s[0]) {
            case '.':
                print("Take it easy!\n");
                return;

            case '#':
                print("` printing the state of the lexer...\n");
                Lexer::print();
                continue;

            case '?':
                print("` printing program tree...\n");
                printProgramTree(program);
                continue;
        }

        Lexer :: tokenize(s, "stdin", line);
        program = Parser :: parse(Lexer::tokens);

        line++;

    } while (true);
}

