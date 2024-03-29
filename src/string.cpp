
#include <stdarg.h> // concat function has varags

#include "alloc.h"
#include "string.h"
#include "types.h"


bool Str_isDigit(char c) {
    return (c >= '0') && (c <= '9');
}

bool Str_isAlpha(char c) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

bool Str_isHexDigit(char c) {
    return ((c >= '0') && (c <= '9'))
        || ((c >= 'A') && (c <= 'F'))
        || ((c >= 'a') && (c <= 'f'));
}

bool Str_isOctalDigit(char c) {
    return (c >= '0') && (c <= '7');
}

bool Str_isBinaryDigit(char c) {
    return (c == '0') || (c == '1');
}

bool Str_isAsciiWhitespace(char c) {
    switch (c) {
        case '\b':
        case '\v':
        case '\f':
        case '\r':
        case '\t':
        case '\n':
        case ' ':
            return true;
        default:
            return false;
    }
}

char* Str_s32ToString(u64 s32eger) {
    u32 capacity = 10;
    u32* remainders = (u32*) pMalloc(sizeof (u32) * capacity);

    u32 count = 0;

    while (true) {
        if (capacity <= count) {
            capacity *= 2;
            remainders = (u32*) pRealloc(remainders, sizeof (u32) * capacity);
        }

        remainders[count++] = s32eger % 10;

        s32eger /= 10;

        if (s32eger == 0) {
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
u64 Str_hexStringTos32(const char* str) {
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

u32 Str_len(const char* string) {
    u32 length = 0;

    while (*string != '\0') {
        length++;
        string++;
    }

    return length;
}

bool Str_eq(const char* s1, const char* s2) {
    u32 l1 = Str_len(s1);
    u32 l2 = Str_len(s2);

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

char* Str_cp(const char* string, u32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * (length + 1));

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = string[i];
    }

    buffer[i] = '\0';

    return buffer;
}

bool Str_memeq(const char* m1, u32 l1, const char* m2, u32 l2) {
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

void* Str_memset(void* p, char c, u32 length) {
    char* a = (char*) p;

    for (u32 i = 0; i < length; i++) {
        a[i] = c;
    }

    return a;
}

void Str_memcpy(void* dest, void* src, u32 size) {
    char* dest_ = (char*) dest;
    char* src_ = (char*) src;

    for (u32 i = 0; i < size; i++) {
        dest_[i] = src_[i];
    }
}

const char* Str_lastCharOccurence(const char* string, u32 length, char c) {
    for (s32 i = length - 1; i >= 0; i--) { // @NOTE 'i' needs to be a signed s32 here...
        if (*(string + i) == c) {
            return string + i;
        }
    }

    return null;
}

bool Str_hasSuffix(const char* string, const char* suffix) {
    const char* p = Str_lastCharOccurence(string, Str_len(string), suffix[0]);

    if (p) {
        return Str_eq(p, suffix);
    }

    return false;
}

u32 Str_countLines(const char* buffer) {
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
bool Str_isAscii(const char* buffer, u32 length) {
    const unsigned char* ubuffer = (const unsigned char*) buffer;

    for (u32 i = 0; i < length; i++) {
        if (ubuffer[i] & 128) { // 0b10000000
            return false;
        }
    }

    return true;
}

// https://www.fileformat.info/info/unicode/category/Zs/list.htm
/* @TODO
bool Str_isUnicodeSpaceSeparator(char c) {
    switch (c) {
        case 0x20:
        case 0xA0:
        case 0x1680:
        case 0x2000:
        case 0x2001:
        case 0x2002:
        case 0x2003:
        case 0x2004:
        case 0x2005:
        case 0x2006:
        case 0x2007:
        case 0x2008:
        case 0x2009:
        case 0x200A:
        case 0x202F:
        case 0x205F:
        case 0x3000:
            return true;

        default:
            return false;
    }
}
*/

/*
 * @TODO ALL OF THESE
 */
//char* Str_trimStart(const char* str, u32 count) {
//    return null;
//}
//
//char* Str_trimEnd(const char* str, u32 count) {
//    return null;
//}

// @TODO test with weird counts
char* Str_trim(const char* str, u32 count) {
    u32 length = Str_len(str);

    if (length <= count) {
        return (char*) "";
    }

    char* buffer = (char*) pMalloc(sizeof (char) * (length - 1));

    u32 i = 0;
    for (; i < (length - count); i++) {
        buffer[i] = str[i + 1];
    }

    buffer[i] = '\0';

    return buffer;
}

char* Str_toLower(const char* str) {
    u32 length = Str_len(str);
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        if (Str_isAlpha(str[i]) && (str[i] < 'a')) {
            buffer[i] = str[i] + 32;

        } else {
            buffer[i] = str[i];
        }
    }

    buffer[i] = '\0';

    return buffer;
}

char* Str_toUpper(const char* str) {
    u32 length = Str_len(str);
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        if (Str_isAlpha(str[i]) && (str[i] > 'Z')) {
            buffer[i] = str[i] - 32;

        } else {
            buffer[i] = str[i];
        }
    }

    buffer[i] = '\0';

    return buffer;
}

char* Str_concat(const char* str1, const char* str2) {
    u32 l1 = Str_len(str1);
    u32 l2 = Str_len(str2);

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

char* Str_concat(u32 argc, ...) {
    va_list args;
    va_start(args, argc);

    char* out = va_arg(args, char*);

    for (u32 i = 1; i < argc; i++) {
        out = Str_concat(out, va_arg(args, char*));
    }

    va_end(args);

    return out;
}

/**
 * Reads |length| characters from |buffer| s32o a newly allocated buffer and returns it.
 * Appends the null character, so the returned string is |length| + 1 in size.
 */
char* Str_read(const char* buffer, u32 length) {
    char* tk = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    while (i < length) {

        tk[i] = *(buffer + i);

        i++;
    }

    tk[i] = '\0';

    return tk;
}

static s32 b64idx(s32 c) {
    if (c < 26) {
        return c + 'A';
    } else if (c < 52) {
        return c - 26 + 'a';
    } else if (c < 62) {
        return c - 52 + '0';
    } else {
        return c == 62 ? '+' : '/';
    }
}

static s32 b64Rev(s32 c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c + 26 - 'a';
    } else if (c >= '0' && c <= '9') {
        return c + 52 - '0';
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    } else if (c == '=') {
        return 64;
    } else {
        return -1;
    }
}

static s32 b64Update(unsigned char ch, char *to, s32 n) {
    unsigned char rem = (n & 3) % 3;
    if (rem == 0) {
        to[n] = b64idx(ch >> 2);
        to[++n] = (ch & 3) << 4;
    } else if (rem == 1) {
        to[n] = b64idx(to[n] | (ch >> 4));
        to[++n] = (ch & 15) << 2;
    } else {
        to[n] = b64idx(to[n] | (ch >> 6));
        to[++n] = b64idx(ch & 63);
        n++;
    }
    return n;
}

static s32 b64Final(char *to, s32 n) {
    s32 saved = n;
    // prs32f("---[%.*s]\n", n, to);
    if (n & 3) n = b64Update(0, to, n);
    if ((saved & 3) == 2) n--;
    // prs32f("    %d[%.*s]\n", n, n, to);
    while (n & 3) to[n++] = '=';
    to[n] = '\0';
    return n;
}

s32 Str_b64Encode(const unsigned char *p, s32 n, char *to) {
    s32 i, len = 0;
    for (i = 0; i < n; i++) len = b64Update(p[i], to, len);
    len = b64Final(to, len);
    return len;
}

s32 Str_b64Decode(const char *src, s32 n, char *dst) {
    const char *end = src + n;
    s32 len = 0;
    while (src + 3 < end) {
        s32 a = b64Rev(src[0]), b = b64Rev(src[1]), c = b64Rev(src[2]),
            d = b64Rev(src[3]);
        if (a == 64 || a < 0 || b == 64 || b < 0 || c < 0 || d < 0) return 0;
        dst[len++] = (a << 2) | (b >> 4);
        if (src[2] != '=') {
            dst[len++] = (b << 4) | (c >> 2);
            if (src[3] != '=') dst[len++] = (c << 6) | d;
        }
        src += 4;
    }
    dst[len] = '\0';
    return len;
}

