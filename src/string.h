
#ifndef STRING_H
#define STRING_H


extern bool isDigit(char c);
extern bool isAlpha(char c);
extern bool isHexDigit(char c);
extern bool isOctalDigit(char c);
extern bool isBinaryDigit(char c);

// different programs consider different codepoints to be 'whitespace'.
extern bool isClueWhitespace(char c);

extern char* intToString(u64 integer); // @TODO move me
extern u64 hexStringToInt(const char* str);

extern u32 strln(const char* string);
extern bool streq(const char* s1, const char* s2);
extern char* strcp(const char* string, u32 length); // more like strdup?

extern bool memeq(const char* m1, u32 l1, const char* m2, u32 l2);
extern void* memset(void* p, char c, u32 length);

extern const char* lastCharOccurence(const char* string, u32 length, char c);
extern bool hasSuffix(const char* string, const char* suffix);

extern u32 countLines(const char* buffer);

extern bool isAscii(const char* buffer, u32 length);

/*
 * @TODO ALL OF THESE
 */
extern char* trimStart(const char* str, u32 count);
extern char* trimEnd(const char* str, u32 count);
extern char* trim(const char* str, u32 count);
extern char* trimQuotes(const char* str, u32 length); // trim with count = 2

extern char* toLower(const char* str);
extern char* toUpper(const char* str);

extern char* concat(const char* str1, const char* str2);
extern char* concat(u32 argc, ...);

/**
 * Reads |length| characters from |buffer| into a newly allocated buffer and returns it.
 * Appends the null character, so the returned string is |length| + 1 in size.
 */
extern char* read(const char* buffer, u32 length);

#endif

