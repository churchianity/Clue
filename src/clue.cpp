
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "runtime.h"
#include "symbol.h"
#include "print.h"
#include "table.h"
#include "util.h"

Table* op_t = NULL;

CommandLineArguments* CLAs = NULL;

/**
 * @TODO
 */
static Table* initGlobalSymbolTable() {
    if (op_t != NULL) {
        return op_t;
    }

    Table *t = newTable(CLUE_GLOBAL_SYMBOL_TABLE_SIZE);

    t->insert(t, ";", newSymbol("", 0, true));
    t->insert(t, ",", newSymbol("", 0, true));
    t->insert(t, ".", newSymbol("", 90, true));
    t->insert(t, "(", newSymbol("", 90, true));
    t->insert(t, ")", newSymbol("", 90, true));
    t->insert(t, "{", newSymbol("", 0, true));
    t->insert(t, "}", newSymbol("", 0, true));
    t->insert(t, "[", newSymbol("", 90, true));
    t->insert(t, "]", newSymbol("", 90, true));

    t->insert(t, "+", newSymbol("", 50, true));
    t->insert(t, "-", newSymbol("", 50, true));
    t->insert(t, "*", newSymbol("", 60, true));
    t->insert(t, "/", newSymbol("", 60, true));
    t->insert(t, "%", newSymbol("", 60, true));

    t->insert(t, "&", newSymbol("", 60, true));
    t->insert(t, "|", newSymbol("", 60, true));
    t->insert(t, "~", newSymbol("", 60, true));
    t->insert(t, "^", newSymbol("", 60, true));

    t->insert(t, "//", newSymbol("", 0, true));
    t->insert(t, "++", newSymbol("", 90, true));
    t->insert(t, "--", newSymbol("", 90, true));
    t->insert(t, "<<", newSymbol("", 60, true));
    t->insert(t, ">>", newSymbol("", 60, true));
    t->insert(t, "**", newSymbol("", 70, true));

    t->insert(t, ":", newSymbol("", 90, true));

    t->insert(t, "=", newSymbol("", 10, true));

    t->insert(t, ":=", newSymbol("", 10, true));
    t->insert(t, "+=", newSymbol("", 10, true));
    t->insert(t, "-=", newSymbol("", 10, true));
    t->insert(t, "*=", newSymbol("", 10, true));
    t->insert(t, "/=", newSymbol("", 10, true));
    t->insert(t, "%=", newSymbol("", 10, true));
    // t->insert(t, "<<=", newSymbol("", 10, true));
    // t->insert(t, ">>=", newSymbol("", 10, true));

    t->insert(t, "&=", newSymbol("", 10, true));
    t->insert(t, "|=", newSymbol("", 10, true));
    t->insert(t, "~=", newSymbol("", 10, true));
    t->insert(t, "^=", newSymbol("", 10, true));

    t->insert(t, "==", newSymbol("", 80, true));
    t->insert(t, ">=", newSymbol("", 80, true));
    t->insert(t, "<=", newSymbol("", 80, true));
    t->insert(t, ">", newSymbol("", 80, true));
    t->insert(t, "<", newSymbol("", 80, true));

    t->insert(t, "&&", newSymbol("", 70, true));
    t->insert(t, "||", newSymbol("", 70, true));

    t->insert(t, "import", newSymbol("", 0, true));
    t->insert(t, "print", newSymbol("", 0, true));

    #if CLUE_DEBUG_LEVEL > 0
        // printf("\n\tGlobal Symbol Table after init:\n%s\n", _DIV);
        // print(t);
    #endif

    return t;
}

/**
 * @TODO
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
    CLAs = (CommandLineArguments*) pCalloc(1, sizeof (CommandLineArguments));

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

        } else if (streq(argv[i], "--project-root")) {
            CLAs->src = argv[i];

        } else if (hasSuffix(argv[i], CLUE_FILE_SUFFIX)) {
            clueFileRead(argv[i]);

        } else {
            help(argv[i]);
        }
    }

    if (!CLAs->src) {
        CLAs->src = "src"; // should be able to set this with a config file as well as CLAs
    }
}

int main(int argc, const char* argv[]) {
    op_t = initGlobalSymbolTable();
    handleCommandLineArguments(argc, argv);

    if (CLAs->interactive) {
        interactive();
    }

    return 0;
}

