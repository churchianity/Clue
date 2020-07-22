
#ifndef MESSAGE_H
#define MESSAGE_H

#include "clue.h"


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

extern const char* messageSeverityToColor(MessageSeverityEnum severity);
extern const char* messageSeverityToString(MessageSeverityEnum severity);

#endif

