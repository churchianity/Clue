
#include "clue.h"
#include "runtime.h"
#include "string.h"
#include "symbol.h"
#include "operator.h"
#include "print.h"
#include "table.h"
#include "util.h"


/**
 * Prints a stack trace.
 */
void trace(FILE* out, u32 maxFrames) {
    void** stack = (void**) pMalloc(sizeof (void*) * maxFrames);
    u32 stackSize = backtrace(stack, maxFrames / sizeof(void*));

    // resolve addresses into strings containing "filename(function+address)"
    // this array must be free()-ed
    char** traces = backtrace_symbols(stack, stackSize);

    if (stackSize < 2) {
        fprintf(stderr, "stack has a weird number (%d) of frames! and we segfaulted anyway sooo...\n", stackSize);
        exit(1);
    }

    // iterate over the returned symbol lines. skip the first, it is the address of this function
    for (int i = 1; i < stackSize; i++) {
        fprintf(out, "  %s\n", traces[i]);
    }

    free(traces);
    free(stack);
}

/**
 * Handler for SIGSEGV.
 */
static void handler(int signal) {
    printf("%serror%s: %d\n", ANSI_RED, ANSI_RESET, signal);
    trace();
    exit(1);
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
    for (int i = 1; i < argc; ++i) {

        // early-exit cases first
        if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            help(NULL); exit(0);

        } else if (streq(argv[i], "-v") || streq(argv[i], "--version")) {
            printf("clue programming language v%s\n\n", CLUE_VERSION_NUMBER); exit(0);

        } else if (streq(argv[i], "-i") || streq(argv[i], "--interactive")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "-s") || streq(argv[i], "--sandbox")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "--project-root")) {
            CLAs.src = argv[i];

        } else if (hasSuffix(argv[i], CLUE_FILE_SUFFIX)) {
            clueFileRead(argv[i]);

        } else {
            help(argv[i]);
        }
    }

    if (!CLAs.src) {
        CLAs.src = "src"; // @TODO should be able to set this with a config file as well as CLAs
    }
}

int main(int argc, const char* argv[]) {
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    handleCommandLineArguments(argc, argv);

    // initGlobalSymbolTable();
    initOperatorTable();

    if (CLAs.interactive) {
        interactive();
    }

    return 0;
}

