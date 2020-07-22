
#ifndef PRINT_H
#define PRINT_H

#include "message.h"
#include "node.h"
#include "table.h"
#include "token.h"
#include "stack.h"


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


void print(const Token* token);
void print(const ASTNode* node);
void print(const Stack* stack);
void print(const Table* table);
void print(const Message* message);

#endif

