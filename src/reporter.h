
#ifndef REPORTER_H
#define REPORTER_H

#include "message.h"


namespace Reporter {
    extern u32 messageCount;
    extern u32 messageCapacity;

    extern Message* messages;

    extern void add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void report(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void flush();
}

#endif

