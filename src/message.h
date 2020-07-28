
#ifndef MESSAGE_H
#define MESSAGE_H

#include "types.h"


enum MessageSeverityEnum {
    MS_LINT,
    MS_WARN,
    MS_ERROR
};

struct Message {
    const char* content;
    const char* functionName;
    const char* filename;

    MessageSeverityEnum severity;

    u32 line;
    u32 column;
};

extern const char* reconstruct(u32 line);
extern const char* messageSeverityToColor(MessageSeverityEnum severity);
extern const char* messageSeverityToString(MessageSeverityEnum severity);

#endif

