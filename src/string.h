
#ifndef STRING_H
#define STRING_H

typedef struct SmallString {
    char data[23];
    u8 remaining;
} SmallString;

typedef struct String {
    union {
    }
    char* data;
} String;

extern bool Str_isDigit(char c);
extern bool Str_isAlpha(char c);
extern bool Str_isHexDigit(char c);
extern bool Str_isOctalDigit(char c);
extern bool Str_isBinaryDigit(char c);

extern char* Str_intToString(u64 integer); // @TODO move me
extern u64 Str_hexStringToInt(const char* str);

extern u32 Str_len(const char* string);
extern bool Str_eq(const char* s1, const char* s2);
extern char* Str_cp(const char* string, u32 length); // more like strdup?

extern bool Str_memeq(const char* m1, u32 l1, const char* m2, u32 l2);
extern void* Str_memset(void* p, char c, u32 length);
extern void Str_memcpy(void* dest, void* src, u32 size);

extern const char* Str_lastCharOccurence(const char* string, u32 length, char c);
extern bool Str_hasSuffix(const char* string, const char* suffix);

extern u32 Str_countLines(const char* buffer);

extern bool Str_isAscii(const char* buffer, u32 length);
extern bool Str_isAsciiWhitespace(char c);
// extern bool isUnicodeSpaceSeparator(wide character);

// @TODO ALL OF THESE TRIMS
//extern char* Str_trimStart(const char* str, u32 count);
//extern char* Str_trimEnd(const char* str, u32 count);
extern char* Str_trim(const char* str, u32 count);

extern char* Str_toLower(const char* str);
extern char* Str_toUpper(const char* str);

extern char* Str_concat(const char* str1, const char* str2);
extern char* Str_concat(u32 argc, ...);

extern char* Str_read(const char* buffer, u32 length);

#endif

