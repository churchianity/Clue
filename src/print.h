
#ifndef PRINT_H
#define PRINT_H

// could use extern variables with c++17... meh
// Text Colors
#define ANSI_BLACK  "\x001B[30m"
#define ANSI_RED    "\x001B[31m"
#define ANSI_GREEN  "\x001B[32m"
#define ANSI_YELLOW "\x001B[33m"
#define ANSI_BLUE   "\x001B[34m"
#define ANSI_PURPLE "\x001B[35m"
#define ANSI_CYAN   "\x001B[36m"
#define ANSI_WHITE  "\x001B[37m"

// Background Colors
#define ANSI_BLACK_BACKGROUND  "\x001B[40m"
#define ANSI_RED_BACKGROUND    "\x001B[41m"
#define ANSI_GREEN_BACKGROUND  "\x001B[42m"
#define ANSI_YELLOW_BACKGROUND "\x001B[43m"
#define ANSI_BLUE_BACKGROUND   "\x001B[44m"
#define ANSI_PURPLE_BACKGROUND "\x001B[45m"
#define ANSI_CYAN_BACKGROUND   "\x001B[46m"
#define ANSI_WHITE_BACKGROUND  "\x001B[47m"

// Wrap Text
#define ANSI_WRAP "\x001B[7m"

// Make text blink
#define ANSI_BLINK "\x001B[5m"

// Clear Console & Reset Cursor Position
#define ANSI_CLEAR "\x001B[2J\x001B[;H"

// Reset Colors
#define ANSI_RESET "\x001B[0m"

/**
 * The entire purpose of this is so we don't have to #import <stdio.h> everywhere
 * +we intend to replace printf at some point with this
 */
extern void print(const char* format, ...);

/**
 * Prints a stack trace.
 */
extern void trace();

/**
 * This should end the program, use for fatal **internal only** errors.
 */
extern void die(const char* format, ...);

extern void print(bool b);
extern void print(char c);
extern void print(signed int i);
extern void print(unsigned int i);
extern void print(float f);
extern void print(double d);
extern void print(void* p);

#endif

