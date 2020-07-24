
#ifndef COLOR_H
#define COLOR_H

// Text Colors
const char* ANSI_BLACK  = "\x001B[30m";
const char* ANSI_RED    = "\x001B[31m";
const char* ANSI_GREEN  = "\x001B[32m";
const char* ANSI_YELLOW = "\x001B[33m";
const char* ANSI_BLUE   = "\x001B[34m";
const char* ANSI_PURPLE = "\x001B[35m";
const char* ANSI_CYAN   = "\x001B[36m";
const char* ANSI_WHITE  = "\x001B[37m";

// Background Colors
const char* ANSI_BLACK_BACKGROUND  = "\x001B[40m";
const char* ANSI_RED_BACKGROUND    = "\x001B[41m";
const char* ANSI_GREEN_BACKGROUND  = "\x001B[42m";
const char* ANSI_YELLOW_BACKGROUND = "\x001B[43m";
const char* ANSI_BLUE_BACKGROUND   = "\x001B[44m";
const char* ANSI_PURPLE_BACKGROUND = "\x001B[45m";
const char* ANSI_CYAN_BACKGROUND   = "\x001B[46m";
const char* ANSI_WHITE_BACKGROUND  = "\x001B[47m";

// Wrap Text
const char* ANSI_WRAP = "\x001B[7m";

// Make text blink
const char* ANSI_BLINK = "\x001B[5m";

// Clear Console & Reset Cursor Position
const char* ANSI_CLEAR = "\x001B[2J\x001B[;H";

// Reset Colors
const char* ANSI_RESET = "\x001B[0m";

#endif

