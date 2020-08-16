
#ifndef REPORTER_H
#define REPORTER_H

#include "array.hpp"
#include "message.h"


namespace Reporter {
    extern void add(u32 id, ASTNode* node);
    extern void add(u32 id, MessageContext context);

    extern void report(u32 id, ASTNode* node);
    extern void report(u32 id, MessageContext context);

    extern void flush();
}

#endif

