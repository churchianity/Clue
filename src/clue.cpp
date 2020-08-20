
#ifdef _WIN32
    #define OS "Windows 32bit"
#endif

#ifdef _WIN64
    #define OS "Windows 64bit"
#endif

#ifdef __linux__
    #define OS "Linux"
#endif

#ifdef __APPLE__
    #define OS "MacOSX"
#endif

#ifdef __FreeBSD__
    #define OS "FreeBSD"
#endif

#ifdef __ANDROID__
    #define OS "Android"
#endif

#include <signal.h> // for signal() - needed on my chromebook for some reason?

#include "clue.h"
#include "print.h"
#include "runtime.h"
#include "string.h"
#include "trace.h"
#include "types.h"
#include "util.h"


/**
 * Handler for SIGSEG, SIGABRT
 */
static void handler(int signal) {
    print("%serror%s: %d\n", ANSI_RED, ANSI_RESET, signal);
    trace();
    exit(1);
}

/**
 * @TODO
 */
static inline void help(const char* arg) {
    if (!arg) { // generic help
        print("this should be some helpful text... but it probably isn't, huh.\n");
        return;
    }

    // specific help
    print("'%s' isn't a valid argument and i should probably help you figure that out but i can't yet.\n", arg);
}

static inline void handleCommandLineArguments(int argc, const char* argv[]) {
    if (argc == 1) {
        help(null); exit(0);
    }

    u32 capacity = 10;
    u32 filec = 0;

    char** files = (char**) pMalloc(sizeof (char*) * capacity);

    for (int i = 1; i < argc; ++i) {
        if (streq(argv[i], "-h") || streq(argv[i], "--help")) {
            help(null); exit(0);

        } else if (streq(argv[i], "-v") || streq(argv[i], "--version")) {
            print("clue programming language v%s, for %s\n", CLUE_VERSION_NUMBER, OS); exit(0);

        } else if (streq(argv[i], "-i") || streq(argv[i], "--interactive")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "-s") || streq(argv[i], "--sandbox")) {
            CLAs.interactive = true;

        } else if (streq(argv[i], "--project-root")) {
            CLAs.src = argv[i];

        } else if (hasSuffix(argv[i], CLUE_FILE_SUFFIX)) {
            u32 size = strln(argv[i]);

            if (filec == capacity) {
                capacity *= 2;
                files = (char**) pRealloc(files, sizeof (char*) * capacity);
            }

            files[filec++] = read(argv[i], size);

        } else {
            help(argv[i]); exit(0);
        }
    }

    CLAs.files = files;
    CLAs.filec = filec;

    if (!CLAs.src) {
        CLAs.src = "src"; // @TODO should be able to set this with a config file as well as CLAs
    }
}

int main(int argc, const char* argv[]) {
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    handleCommandLineArguments(argc, argv);

    for (s32 i = 0; i < CLAs.filec; i++) {
        doIt(clueFileRead(CLAs.files[i]), CLAs.files[i]);
    }

    if (CLAs.interactive) {
        interactive();
    }

    return 0;
}

