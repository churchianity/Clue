
#include <stdlib.h> // exit

#include "print.h"


const char* messageSeverityToColor(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return ANSI_BLUE;
        case MS_WARN: return ANSI_YELLOW;
        case MS_ERROR: return ANSI_RED;
        default: print("message severity passed that doesn't exist: |%d|\n", &severity); exit(1);
    }
}

const char* messageSeverityToString(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return "lint";
        case MS_WARN: return "warn";
        case MS_ERROR: return "error";
        default: print("message severity passed that doesn't exist: |%d|\n", &severity); exit(1);
    }
}

