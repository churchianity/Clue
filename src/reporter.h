
#ifndef REPORTER_H
#define REPORTER_H

#include <stdarg.h> // va_list

#include "array.hpp"
#include "message.h"
#include "node.h"
#include "token.h"

/**
 * This set of functions lets you interact with the reporter by adding messages.
 *
 * flush() empties the internal array and shows you everything.
 * add() appends to the array.
 * report() calls add(), then calls flush(), then calls exit(1) (when not in debug mode, if in debug mode it tries to stay alive)
 */
extern void Reporter_flush();

extern void Reporter_add(u32 id, ASTNode* node, ...);
extern void Reporter_add(u32 id, const char* functionName, Token* token, ...);
extern void Reporter_add(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...);

extern void Reporter_report(u32 id, ASTNode* node, ...);
extern void Reporter_report(u32 id, const char* functionName, Token* token, ...);
extern void Reporter_report(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...);

extern void Reporter_rebuild(const char* filename);

#endif

