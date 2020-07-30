
#ifndef PRINT_H
#define PRINT_H

#include "message.h"
#include "node.h"
#include "table.hpp"
#include "token.h"
#include "stack.hpp"


// Text Colors
extern const char* ANSI_BLACK;
extern const char* ANSI_RED;
extern const char* ANSI_GREEN;
extern const char* ANSI_YELLOW;
extern const char* ANSI_BLUE;
extern const char* ANSI_PURPLE;
extern const char* ANSI_CYAN;
extern const char* ANSI_WHITE;

// Background Colors
extern const char* ANSI_BLACK_BACKGROUND;
extern const char* ANSI_RED_BACKGROUND;
extern const char* ANSI_GREEN_BACKGROUND;
extern const char* ANSI_YELLOW_BACKGROUND;
extern const char* ANSI_BLUE_BACKGROUND;
extern const char* ANSI_PURPLE_BACKGROUND;
extern const char* ANSI_CYAN_BACKGROUND;
extern const char* ANSI_WHITE_BACKGROUND;

// Wrap Text
extern const char* ANSI_WRAP;

// Make text blink
extern const char* ANSI_BLINK;

// Clear Console & Reset Cursor Position
extern const char* ANSI_CLEAR;

// Reset Colors
extern const char* ANSI_RESET;

/**
 * This should end the program, use for fatal internal errors.
 */
extern void die(const char* formatString, ...);

/**
 * @NOTE because we want to replace printf at some point maybe, I'm keeping all existing printf calls routed through here.
 */
extern void print(const char* fmt, ...);

extern void print(const Token* token);
extern void print(const Symbol* symbol);

extern void print(const ASTNode* node);

extern void print(const Message* message);

template <class T>
extern void print(const Stack<T>* stack);

template <class K, class V>
extern void print(const Table<K, V>* table);

#endif

