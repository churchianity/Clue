
#ifndef REPORTER_H
#define REPORTER_H

#include "array.hpp"
#include "message.h"


namespace Reporter {
    extern Array<Message>* messages;

    extern void add(u32 id, ASTNode* node);
    extern void add(u32 id, MessageContext context);

    extern void report(u32 id, ASTNode* node);
    extern void report(u32 id, MessageContext context);

    extern void add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);
    extern void report(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column);

    extern void flush();
}

#endif

