
#include <stdarg.h> // concat function has varags

#include "alloc.h"
#include "types.h"


bool isDigit(char c) {
    return (c >= '0') && (c <= '9');
}

bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

bool isHexDigit(char c) {
    return ((c >= '0') && (c <= '9'))
        || ((c >= 'A') && (c <= 'F'))
        || ((c >= 'a') && (c <= 'f'));
}

bool isOctalDigit(char c) {
    return (c >= '0') && (c <= '7');
}

bool isBinaryDigit(char c) {
    return (c == '0') || (c == '1');
}

// different programs consider different codepoints to be 'whitespace'.
// we only care about these.
bool isClueWhitespace(char c) {
    return (c == ' ') || (c == '\n') || (c == '\t') || (c == '\r');
}

char* intToString(u64 integer) {
    u32 capacity = 10;
    u32* remainders = (u32*) pMalloc(sizeof (u32) * capacity);

    u32 count = 0;

    while (true) {
        if (capacity <= count) {
            capacity *= 2;
            remainders = (u32*) pRealloc(remainders, sizeof (u32) * capacity);
        }

        remainders[count++] = integer % 10;

        integer /= 10;

        if (integer == 0) {
            break;
        }
    }

    char* buffer = (char*) pMalloc(sizeof (char) * count + 1);

    for (u32 i = 0; i < count; i++) {
        buffer[count - i - 1] = '0' + remainders[i];
    }

    buffer[count] = '\0';

    pFree(remainders);

    return buffer;
}

// @NOTE breaks if you don't verify the string is a valid hex string.
u64 hexStringToInt(const char* str) {
    u64 out = 0;

    while (*str != '\0') {
        u8 byte = *str++;

        if ((byte >= '0') && (byte <= '9')) {
            byte = byte - '0';

        } else if ((byte >= 'a') && (byte <= 'f')) {
            byte = byte - 'a' + 10;

        } else if ((byte >= 'A') && (byte <= 'F')) {
            byte = byte - 'A' + 10;
        }

        // only use the last four bits - precision of a single hex digit
        out = (out << 4) | (byte & 0xF);
    }

    return out;
}

u32 strln(const char* string) {
    u32 length = 0;

    while (*string != '\0') {
        length++;
        string++;
    }

    return length;
}

bool streq(const char* s1, const char* s2) {
    u32 l1 = strln(s1);
    u32 l2 = strln(s2);

    if (l1 != l2) {
        return false;
    }

    for (u32 i = 0; i < l1; i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }

    return true;
}

char* strcp(const char* string, u32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * (length + 1));

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = string[i];
    }

    buffer[i] = '\0';

    return buffer;
}

bool memeq(const char* m1, u32 l1, const char* m2, u32 l2) {
    if (l1 != l2) {
        return false;
    }

    for (u32 i = 0; i < l1; i++) {
        if (m1[i] != m2[i]) {
            return false;
        }
    }

    return true;
}

void* memset(void* p, char c, u32 length) {
    char* a = (char*) p;

    for (u32 i = 0; i < length; i++) {
        a[i] = c;
    }

    return a;
}

const char* lastCharOccurence(const char* string, u32 length, char c) {
    for (s32 i = length - 1; i >= 0; i--) { // @NOTE 'i' needs to be a signed int here...
        if (*(string + i) == c) {
            return string + i;
        }
    }

    return null;
}

bool hasSuffix(const char* string, const char* suffix) {
    const char* p = lastCharOccurence(string, strln(string), suffix[0]);

    if (p) {
        return streq(p, suffix);
    }

    return false;
}

u32 countLines(const char* buffer) {
    u32 lines = 0;
    char c;

    while ((c = *buffer) != '\0') {
        if (c == '\n') {
            lines++;
        }

        buffer++;
    }

    return lines;
}

/**
 * Ascii encoded text won't ever set the 8th (big) bit of any of its bytes.
 * @TODO is |length| necessary here?
 */
bool isAscii(const char* buffer, u32 length) {
    const unsigned char* ubuffer = (const unsigned char*) buffer;

    for (u32 i = 0; i < length; i++) {
        if (ubuffer[i] & 128) { // binary: 0b 1000 0000
            return false;
        }
    }

    return true;
}

/*
 * @TODO ALL OF THESE
 */
char* trimStart(const char* str, u32 count) {
    return null;
}

char* trimEnd(const char* str, u32 count) {
    return null;
}

// @TODO test with weird counts
char* trim(const char* str, u32 count) {
    u32 length = strln(str);
    char* buffer = (char*) pMalloc(sizeof (char) * (length - 1));

    s32 i = 0;
    for (; i < (length - count); i++) {
        buffer[i] = str[i + 1];
    }

    buffer[i] = '\0';

    return buffer;
}

char* toLower(const char* str) {
    u32 length = strln(str);
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        if (isAlpha(str[i]) && (str[i] < 'a')) {
            buffer[i] = str[i] + 32;

        } else {
            buffer[i] = str[i];
        }
    }

    buffer[i] = '\0';

    return buffer;
}

char* toUpper(const char* str) {
    u32 length = strln(str);
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        if (isAlpha(str[i]) && (str[i] > 'Z')) {
            buffer[i] = str[i] - 32;

        } else {
            buffer[i] = str[i];
        }
    }

    buffer[i] = '\0';

    return buffer;
}

char* concat(const char* str1, const char* str2) {
    u32 l1 = strln(str1);
    u32 l2 = strln(str2);

    u32 newLength = l1 + l2;

    char* newBuffer = (char*) pMalloc(sizeof (char) * newLength + 1);

    u32 i = 0;
    for (; i < newLength; i++) {
        if (i < l1) {
            newBuffer[i] = str1[i];

        } else {
            newBuffer[i] = str2[i - l1];
        }
    }

    newBuffer[i] = '\0';

    return newBuffer;
}

char* concat(u32 argc, ...) {
    va_list args;
    va_start(args, argc);

    char* out = va_arg(args, char*);

    for (u32 i = 1; i < argc; i++) {
        out = concat(out, va_arg(args, char*));
    }

    va_end(args);

    return out;
}

/**
 * Reads |length| characters from |buffer| into a newly allocated buffer and returns it.
 * Appends the null character, so the returned string is |length| + 1 in size.
 */
char* read(const char* buffer, u32 length) {
    char* tk = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    while (i < length) {

        tk[i] = *(buffer + i);

        i++;
    }

    tk[i] = '\0';

    return tk;
}

