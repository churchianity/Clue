
#if defined(_WIN32)
    #define __CLUE_OS__"Windows 32bit"
#elif defined(_WIN64)
    #define __CLUE_OS__ "Windows 64bit"
#elif defined(__linux__)
    #define __CLUE_OS__ "Linux"
#elif defined(__APPLE__)
    #define __CLUE_OS__ "MacOSX"
#elif defined(__FreeBSD__)
    #define __CLUE__OS__ "FreeBSD"
#elif defined(__ANDROID__)
    #define __CLUE_OS__ "Android"
#else
    #define __CLUE_OS__ "Unknown os"
#endif

// gcc specific
#if defined(__i386__)
    #define __CLUE_ARCH__ "i386"
#elif defined(__x86_64__)
    #define __CLUE_ARCH__ "x86_64"
#else
    #define __CLUE_ARCH__ "unknown architecture"
#endif

#include <stdlib.h> // exit
#include <signal.h> // for signal() - needed on my chromebook for some reason?
#include <execinfo.h> // backtrace, backtrace_symbols
#include <limits.h> // CHAR_BIT // need to investigate this more for robustness

#include "clue.h"
#include "file.h"
#include "print.h"
#include "runtime.h"
#include "string.h"
#include "types.h"


static void assert(bool condition, const char* message = "assertion failed") {
    if (!condition) {
        print(message);
        trace();
        exit(1);
    }
}

static void handler(s32 signal) {
    print("%serror%s: %d\n", ANSI_RED, ANSI_RESET, signal);
    trace();
    exit(1);
}

static inline void help(const char* arg) {
    if (!arg) { // generic help
        print("\n");
        print("Usage: clue <options*> main.clue\n");
        print("  you usually will run clue with just a single file argument, which is the file containing the 'main' function for your program\n");
        print("  files don't need to be named anything in particular, aside from having the .clue suffix\n");
        print("  imports are pulled in and compiled automatically - there's no need to provide them in the argument list\n");
        print("  if you do provide multiple files, they will be treated as separate programs, and multiple executables will be produced\n");
        print("\n");
        print("Options:\n");
        print("  -h --help <option?>    - if an option listed below is provided, provide additional info on that option, otherwise show this message\n");
        print("  -v --version           - show clue version information\n");
        print("  -i --interactive       - after reading in provided source files (if present), enter an interactive mode\n");
        print("  -s --sandbox           - alias for --interactive\n");
        print("  -r --root <option?>    - specifiy a path which is the root directory of your project - this helps with finding files during imports\n");
        print("\n");
        return;
    }

    // specific help
    print("'%s' isn't a valid argument and i should probably help you figure that out but i can't yet.\n", arg);
}

static inline void handleCommandLineArguments(s32 argc, const char* argv[]) {
    if (argc == 1) {
        help(null); exit(0);
    }

    u32 capacity = 10;
    u32 filec = 0;

    char** files = (char**) pMalloc(sizeof (char*) * capacity);

    // skip the first arg, it's the name of the program itself.
    for (s32 i = 1; i < argc; ++i) {
        char* arg = Str :: toLower(argv[i]);

        if (Str :: eq(arg, "-h") || Str :: eq(arg, "--help")) {
            help(null); exit(0);

        } else if (Str :: eq(arg, "-v") || Str :: eq(arg, "--version")) {
            print("clue programming language v%s, for %s, %s\n", __CLUE_VERSION_NUMBER__, __CLUE_OS__, __CLUE_ARCH__); exit(0);

        } else if (Str :: eq(arg, "-i") || Str :: eq(arg, "--interactive")) {
            CLAs.interactive = true;

        } else if (Str :: eq(arg, "-s") || Str :: eq(arg, "--sandbox")) {
            CLAs.interactive = true;

        } else if (Str :: eq(arg, "-r") || Str :: eq(arg, "--root")) {
            CLAs.src = arg;

        } else if (Str :: hasSuffix(arg, CLUE_FILE_SUFFIX)) {
            u32 size = Str :: len(arg);

            if (filec == capacity) {
                capacity *= 2;
                files = (char**) pRealloc(files, sizeof (char*) * capacity);
            }

            files[filec++] = Str :: read(arg, size);

        } else {
            help(arg); exit(0);
        }
    }

    CLAs.files = files;
    CLAs.filec = filec;

    if (!CLAs.src) {
        CLAs.src = "src"; // @TODO should be able to set this with a config file as well as CLAs
    }
}

s32 main(s32 argc, const char* argv[]) {
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    assert(CHAR_BIT == 8);

    handleCommandLineArguments(argc, argv);

    for (u32 i = 0; i < CLAs.filec; i++) {
        Runtime :: doIt(clueFileRead(CLAs.files[i]), CLAs.files[i]);
    }

    if (CLAs.interactive) {
        Runtime :: interactive();
    }

    return 0;
}

