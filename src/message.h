
#ifndef MESSAGE_H
#define MESSAGE_H

#include "clue.h"
#include "print.h"


typedef enum {
    MS_LINT,
    MS_WARN,
    MS_ERROR
} MessageSeverityEnum;

typedef struct {
    MessageSeverityEnum severity;

    const char* content;
    const char* functionName;
    const char* filename;

    u32 line;
    u32 column;

    // @TODO some way to determine context string
} Message;

inline const char* messageSeverityToColor(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return ANSI_BLUE;
        case MS_WARN: return ANSI_YELLOW;
        case MS_ERROR: return ANSI_RED;
        default: fprintf(stderr, "message severity passed that doesn't exist: |%d|\n", severity); exit(1);
    }
}

inline const char* messageSeverityToString(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return "lint";
        case MS_WARN: return "warn";
        case MS_ERROR: return "error";
        default: fprintf(stderr, "message severity passed that doesn't exist: |%d|\n", severity); exit(1);
    }
}

#endif

