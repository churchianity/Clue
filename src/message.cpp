
#include "lexer.h"
#include "string.h"
#include "print.h"


/**
 * @TODO i should just make a memset() like thingy in string.h
 */
static inline char* fillWithSpaces(u32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = ' ';
    }

    buffer[i] = '\0';

    return buffer;
}

/**
 * Because atm we free the buffers containing all of the read-in source code,
 * we have to reconstruct code segments for linter/warning/error message context.
 *
 * Tokens can do this because they store where they are in the file + how long they are.
 *
 * @TODO support multiple lines of context via an offset, above or below or both.
 * @STATEFUL
 */
const char* reconstruct(const char* filename, u32 line) {
    // find the first token in the request file, on the requested line
    const Token* token = null;

    s32 columnSoFar = 0;
    s32 amountOfLeadingWhitespace = 0;

    const char* out = "";

    u32 i = 0;
    for (; i < Lexer::tokenCount; i++) {

        if (streq(Lexer::tokens[i].filename, filename) && (Lexer::tokens[i].line == line)) {
            token = Lexer::tokens + i;

            // the -1 is because column counts are 1-indexed
            amountOfLeadingWhitespace += (token->column - 1) - columnSoFar;
            columnSoFar += (token->column - 1) + token->length;

            char* leadingWhitespace = fillWithSpaces(amountOfLeadingWhitespace);

            out = concat(3, out, leadingWhitespace, token->tk);
        }
    }

    return out;
}

const char* messageSeverityToColor(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return ANSI_BLUE;
        case MS_WARN: return ANSI_YELLOW;
        case MS_ERROR: return ANSI_RED;
        default: die("bad message severity\n"); return null;
    }
}

const char* messageSeverityToString(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return "lint";
        case MS_WARN: return "warn";
        case MS_ERROR: return "error";
        default: die("bad message sevevity\n"); return null;
    }
}

