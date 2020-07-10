
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "runtime.h"
#include "symbol.h"
#include "print.h"
#include "table.h"
#include "util.h"

Table* globalSymbolTable = NULL;

CommandLineArguments* CLAs = NULL;

/**
 *
 */
static Table* initGlobalSymbolTable() {
    if (globalSymbolTable != NULL) {
        return globalSymbolTable;
    }

    Table *t = newTable(CLUE_GLOBAL_SYMBOL_TABLE_SIZE);

    t->insert(t, ",", newSymbol("COMMA", 0, true));

    t->insert(t, "+", newSymbol("PLUS", 50, true));
    t->insert(t, "-", newSymbol("MINUS", 50, true));
    t->insert(t, "*", newSymbol("TIMES", 60, true));
    t->insert(t, "/", newSymbol("DIVIDE", 60, true));
    t->insert(t, "%", newSymbol("MODULUS", 60, true));

    t->insert(t, "<<", newSymbol("LEFT_SHIFT", 60, true));
    t->insert(t, ">>", newSymbol("RIGHT_SHIFT", 60, true));

    t->insert(t, "&", newSymbol("BITWISE_AND", 60, true));
    t->insert(t, "|", newSymbol("BITWISE_OR", 60, true));
    t->insert(t, "~", newSymbol("BITWISE_NOT", 60, true));
    t->insert(t, "^", newSymbol("BITWISE_XOR", 60, true));

    t->insert(t, "=", newSymbol("EQUALS", 10, true));
    t->insert(t, "+=", newSymbol("PLUS_EQUALS", 10, true));
    t->insert(t, "-=", newSymbol("MINUS_EQUALS", 10, true));
    t->insert(t, "*=", newSymbol("TIMES_EQUALS", 10, true));
    t->insert(t, "/=", newSymbol("DIVIDE_EQUALS", 10, true));
    t->insert(t, "%=", newSymbol("MODULUS_EQUALS", 10, true));
    t->insert(t, "<<=", newSymbol("LEFT_SHIFT_ASSIGNMENT", 10, true));
    t->insert(t, ">>=", newSymbol("RIGHT_SHIFT_ASSIGNMENT", 10, true));

    t->insert(t, "&=", newSymbol("BITWISE_AND_EQUALS", 10, true));
    t->insert(t, "|=", newSymbol("BITWISE_OR_EQUALS", 10, true));
    t->insert(t, "~=", newSymbol("BITWISE_NOT_EQUALS", 10, true));
    t->insert(t, "^=", newSymbol("BITWISE_XOR_EQUALS", 10, true));

    t->insert(t, "==", newSymbol("EQUALITY", 80, true));
    t->insert(t, ">=", newSymbol("GREATER_THAN_OR_EQUAL", 80, true));
    t->insert(t, "<=", newSymbol("LESS_THAN_OR_EQUAL", 80, true));
    t->insert(t, ">", newSymbol("GREATER_THAN", 80, true));
    t->insert(t, "<", newSymbol("LESS_THAN", 80, true));

    t->insert(t, "&&", newSymbol("LOGICAL_AND", 70, true));
    t->insert(t, "||", newSymbol("LOGICAL_OR", 70, true));
    t->insert(t, "^^", newSymbol("LOGICAL_XOR", 70, true));

    t->insert(t, ";", newSymbol("SEMICOLON", 0, true));

    t->insert(t, "(", newSymbol("OPEN_PARENS", 0, true));
    t->insert(t, ")", newSymbol("CLOSE_PARENS", 0, true));
    // t->insert(t, "[", newSymbol("OPEN_BRACKET", 0, true));
    // t->insert(t, "]", newSymbol("CLOSE_BRACKET", 0, true));
    // t->insert(t, "{", newSymbol("OPEN_BRACE", 0, true));
    // t->insert(t, "}", newSymbol("CLOSE_BRACE", 0, true));

    t->insert(t, "print", newSymbol("PRINTF", 0, true));

    #if CLUE_DEBUG_LEVEL > 0
        printf("\n\tGlobal Symbol Table after init:\n%s\n", _DIV);
        print(t);
    #endif

    return t;
}

/**
 *
 */
static void help(const char* arg) {
    if (!arg) {
        // generic help
        printf("this should be some helpful text... but it probably isn't, huh.\n");
        return;
    }

    // specific help
    printf("'%s' isn't a valid argument and i should probably help you figure that out but i can't yet.\n", arg);
}

static void handleCommandLineArguments(int argc, const char* argv[]) {
    CLAs = (CommandLineArguments*) pMalloc(sizeof (CommandLineArguments));

    #if CLUE_DEBUG_LEVEL > 1
        if (argc > 1) {
            printf("\nCLA's:\n%s\n", _DIV);
        }
    #endif

    for (int i = 1; i < argc; ++i) {
        #if CLUE_DEBUG_LEVEL > 1
            printf("args[%d]: %s\n", i, argv[i]);
        #endif

        // early-exit cases first
        if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            help(NULL); exit(0);

        } else if (streq(argv[i], "-v") || streq(argv[i], "--version")) {
            printf("clue programming language v%s\n\n", CLUE_VERSION_NUMBER); exit(0);

        } else if (streq(argv[i], "-i") || streq(argv[i], "--interactive")) {
            CLAs->interactive = true;

        } else if (streq(argv[i], "-s") || streq(argv[i], "--sandbox")) {
            CLAs->interactive = true;

        } else if (hasSuffix(argv[i], CLUE_FILE_SUFFIX)) {
            char* buffer = fileRead(argv[i]);
            doIt(buffer, argv[i]);
            free(buffer);

        } else {
            help(argv[i]);
        }
    }
}

int main(int argc, const char* argv[]) {
    globalSymbolTable = initGlobalSymbolTable();
    handleCommandLineArguments(argc, argv);

    if (CLAs->interactive) {
        interactive();
    }

    return 0;
}

