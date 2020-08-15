
#ifndef PRINT_H
#define PRINT_H

#include "node.h"
#include "table.hpp"
#include "token.h"
#include "runtime.h"
#include "array.hpp"


//#region ANSI ESCAPE SEQUENCES
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

//#endregion


/**
 * @NOTE because we want to replace printf at some point maybe, I'm keeping all existing printf calls routed through here.
 */
extern void print(const char* format, ...);

extern void prints(const char* s);
extern void print(bool b);
extern void print(char c);
extern void print(signed int i);
extern void print(unsigned int i);
extern void print(float f);
extern void print(double d);
extern void print(void* v);

extern void print(Value v);

/**
 * This should end the program, use for fatal internal errors.
 */
extern void die(const char* format, ...);

extern void print(const Token* token);
extern void print(const Symbol* symbol);
extern void print(const ASTNode* node);

template <class K, class V>
extern void print(TableEntry<K, V>* entry);

#endif

