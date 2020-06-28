
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clue.h"
#include "runtime.h"
#include "symbol.h"
#include "table.h"
#include "util.h"

/**
 *
 */
CommandLineArguments* CLAs = NULL;

/**
 * All symbols present in the global namespace will be an entry in this table.
 */
Table* globalSymbolTable = NULL;

static void initGlobalSymbolTable() {
    if (globalSymbolTable) {
        return;
    }

    globalSymbolTable = newTable(CLUE_GLOBAL_SYMBOL_TABLE_SIZE);

    globalSymbolTable->insert(globalSymbolTable, ",", newSymbol("COMMA", 0, true));

    globalSymbolTable->insert(globalSymbolTable, "+", newSymbol("PLUS", 50, true));
    globalSymbolTable->insert(globalSymbolTable, "-", newSymbol("MINUS", 50, true));
    globalSymbolTable->insert(globalSymbolTable, "*", newSymbol("TIMES", 60, true));
    globalSymbolTable->insert(globalSymbolTable, "/", newSymbol("DIVIDE", 60, true));
    globalSymbolTable->insert(globalSymbolTable, "%", newSymbol("MODULUS", 60, true));

    globalSymbolTable->insert(globalSymbolTable, "<<", newSymbol("LEFT_SHIFT", 60, true));
    globalSymbolTable->insert(globalSymbolTable, ">>", newSymbol("RIGHT_SHIFT", 60, true));

    globalSymbolTable->insert(globalSymbolTable, "&", newSymbol("BITWISE_AND", 60, true));
    globalSymbolTable->insert(globalSymbolTable, "|", newSymbol("BITWISE_OR", 60, true));
    globalSymbolTable->insert(globalSymbolTable, "~", newSymbol("BITWISE_NOT", 60, true));
    globalSymbolTable->insert(globalSymbolTable, "^", newSymbol("BITWISE_XOR", 60, true));

    globalSymbolTable->insert(globalSymbolTable, "=", newSymbol("EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "+=", newSymbol("PLUS_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "-=", newSymbol("MINUS_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "*=", newSymbol("TIMES_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "/=", newSymbol("DIVIDE_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "%=", newSymbol("MODULUS_EQUALS", 10, true));

    globalSymbolTable->insert(globalSymbolTable, "&=", newSymbol("BITWISE_AND_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "|=", newSymbol("BITWISE_OR_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "~=", newSymbol("BITWISE_NOT_EQUALS", 10, true));
    globalSymbolTable->insert(globalSymbolTable, "^=", newSymbol("BITWISE_XOR_EQUALS", 10, true));

    globalSymbolTable->insert(globalSymbolTable, "==", newSymbol("EQUALITY", 80, true));
    globalSymbolTable->insert(globalSymbolTable, ">=", newSymbol("GREATER_THAN_OR_EQUAL", 80, true));
    globalSymbolTable->insert(globalSymbolTable, "<=", newSymbol("LESS_THAN_OR_EQUAL", 80, true));
    globalSymbolTable->insert(globalSymbolTable, ">", newSymbol("GREATER_THAN", 80, true));
    globalSymbolTable->insert(globalSymbolTable, "<", newSymbol("LESS_THAN", 80, true));

    globalSymbolTable->insert(globalSymbolTable, "&&", newSymbol("LOGICAL_AND", 70, true));
    globalSymbolTable->insert(globalSymbolTable, "||", newSymbol("LOGICAL_OR", 70, true));

    globalSymbolTable->insert(globalSymbolTable, ";", newSymbol("SEMICOLON", 0, true));

    globalSymbolTable->insert(globalSymbolTable, "(", newSymbol("OPEN_PARENS", 0, true));
    globalSymbolTable->insert(globalSymbolTable, ")", newSymbol("CLOSE_PARENS", 0, true));
    globalSymbolTable->insert(globalSymbolTable, "[", newSymbol("OPEN_BRACKET", 0, true));
    globalSymbolTable->insert(globalSymbolTable, "]", newSymbol("CLOSE_BRACKET", 0, true));
    globalSymbolTable->insert(globalSymbolTable, "{", newSymbol("OPEN_BRACE", 0, true));
    globalSymbolTable->insert(globalSymbolTable, "}", newSymbol("CLOSE_BRACE", 0, true));

    globalSymbolTable->insert(globalSymbolTable, "print", newSymbol("PRINTF", 0, true));

    #if CLUE_DEBUG_LEVEL > 1
        printf("\n\tGlobal Symbol Table after init:\n%s\n", _DIV);
        globalSymbolTable->print(globalSymbolTable);
    #endif
}

static void help(const char* arg) {
    if (!arg) {
        // generic help
        printf("this should be some helpful text... but it probably isn't, huh.\n");
        return;
    }

    // specific help
    printf("'%s' isn't a valid argument and i should probably help you figure that out but i can't yet.\n", arg);
}

static char* clueFileRead(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char* buffer = NULL;
    long length;

    if (!fp) {
        fprintf(stderr, "failed to get fp for filename: %s\n", filename);
        exit(1);
    }

    length = getFileSize(fp);
    buffer = pMalloc(length);

    fread(buffer, 1, length, fp);
    fclose(fp);

    return buffer;
}

static void handleCommandLineArguments(int argc, const char* argv[]) {
    CLAs = pMalloc(sizeof (CommandLineArguments));

    CLAs->filenameCount = 0;
    CLAs->filenames = NULL;

    #if CLUE_DEBUG_LEVEL > 1
        if (argc > 1) {
            printf("\nCLA's:\n%s\n", _DIV);
        }
    #endif

    for (unsigned int i = 1; i < argc; ++i) {
        #if CLUE_DEBUG_LEVEL > 1
            printf("args[%d]: %s\n", i, argv[i]);
        #endif

        // early-exit cases first
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            help(NULL); exit(0);

        } else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            printf("clue programming language v%s\n\n", CLUE_VERSION_NUMBER); exit(0);

        } else if ((strcmp(argv[i], "-i") == 0) || (strcmp(argv[i], "--interactive") == 0)) {
            CLAs->interactive = true;

        } else if (hasSuffix(argv[i], CLUE_FILE_SUFFIX)) {
            CLAs->filenameCount++;
            clueFileRead(argv[i]);

        } else {
            help(argv[i]);
        }
    }
}

int main(int argc, const char* argv[]) {
    handleCommandLineArguments(argc, argv);

    initGlobalSymbolTable();

    if (CLAs->interactive) {
        interactive();
    }

    return 0;
}

