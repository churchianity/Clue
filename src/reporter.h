
#ifndef REPORTER_H
#define REPORTER_H

#include "clue.h"
#include "message.h"


namespace Reporter {
    extern Message* messages;

    extern void add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void report(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void flush();
}

extern const char* reconstruct(u32 line);

#endif

