
#include <stdio.h> // stdin

#include "runtime.h"
#include "print.h"
#include "types.h"


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
        }

        // @TODO


        line++;

    } while (true);
}

