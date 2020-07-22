
#ifndef REPORTER_H
#define REPORTER_H

#include <stdio.h>

#include "clue.h"
#include "message.h"


namespace Reporter {
    extern Message* messages;

    extern void flush();
    extern void add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
}

#endif

