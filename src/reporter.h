
#ifndef REPORTER_H
#define REPORTER_H

#include <stdarg.h> // va_list

#include "array.hpp"
#include "message.h"
#include "node.h"
#include "token.h"



namespace Reporter {
    extern void rebuild(const char* filename);
    /**
     * This set of functions lets you interact with the reporter by adding messages.
     *
     * flush() empties the internal array and shows you everything.
     * add() appends to the array.
     * report() calls add(), then calls flush(), then calls exit(1) (when not in debug mode, if in debug mode it tries to stay alive)
     */
    extern void flush();

    extern void add(u32 id, ASTNode* node, ...);
    extern void add(u32 id, const char* functionName, Token* token, ...);
    extern void add(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...);

    extern void report(u32 id, ASTNode* node, ...);
    extern void report(u32 id, const char* functionName, Token* token, ...);
    extern void report(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...);
}

#endif

