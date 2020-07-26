
#include <signal.h>

#include "operator.h"
#include "print.h"
#include "runtime.h"
#include "string.h"
#include "trace.h"
#include "types.h"


/**
 * Handler for SIGSEG, SIGABRT
 */
static void handler(int signal) {
    printf("%serror%s: %d\n", ANSI_RED, ANSI_RESET, signal);
    trace();
    exit(1);
}

/**
 * @TODO
 */
static inline void help(const char* arg) {
    if (!arg) {
        // generic help
        printf("this should be some helpful text... but it probably isn't, huh.\n");
        return;
    }

    // specific help
    printf("'%s' isn't a valid argument and i should probably help you figure that out but i can't yet.\n", arg);
}

static inline void handleCommandLineArguments(int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {

        // early-exit cases first
        if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            help(null); exit(0);

        } else if (streq(argv[i], "-v") || streq(argv[i], "--version")) {
            printf("clue programming language v%s\n\n", CLUE_VERSION_NUMBER); exit(0);

        } else if (streq(argv[i], "-i") || streq(argv[i], "--interactive")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "-s") || streq(argv[i], "--sandbox")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "--project-root")) {
            CLAs.src = argv[i];

        #define CLUE_FILE_SUFFIX ".clue"
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
    signal(11, handler); // SIGSEGV
    signal(6, handler);  // SIGABRT

    handleCommandLineArguments(argc, argv);

    // if we're here, we probably want to compile and/or run some code.
    if (CLAs.interactive) {
        interactive();
    }

    return 0;
}

