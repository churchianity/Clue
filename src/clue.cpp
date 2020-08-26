
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
#include "file.h"
#include "print.h"
#include "runtime.h"
#include "string.h"
#include "trace.h"
#include "types.h"


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

static inline void handleCommandLineArguments(int argc, const char* argv[]) {
    if (argc == 1) {
        help(null); exit(0);
    }

    u32 capacity = 10;
    u32 filec = 0;

    char** files = (char**) pMalloc(sizeof (char*) * capacity);

    for (int i = 1; i < argc; ++i) {
        char* arg = toLower(argv[i]);

        if (streq(arg, "-h") || streq(arg, "--help")) {
            help(null); exit(0);

        } else if (streq(arg, "-v") || streq(arg, "--version")) {
            print("clue programming language v%s, for %s\n", CLUE_VERSION_NUMBER, OS); exit(0);

        } else if (streq(arg, "-i") || streq(arg, "--interactive")) {
            CLAs.interactive = true;

        } else if (streq(arg, "-s") || streq(arg, "--sandbox")) {
            CLAs.interactive = true;

        } else if (streq(arg, "-r") || streq(arg, "--root")) {
            CLAs.src = arg;

        } else if (hasSuffix(arg, CLUE_FILE_SUFFIX)) {
            u32 size = strln(arg);

            if (filec == capacity) {
                capacity *= 2;
                files = (char**) pRealloc(files, sizeof (char*) * capacity);
            }

            files[filec++] = read(arg, size);

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

