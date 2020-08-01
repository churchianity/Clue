
#ifndef REPORTER_H
#define REPORTER_H

#include "clue.h"


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

namespace Reporter {
    extern Message* messages;

    extern void add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void report(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void flush();
}

#endif

