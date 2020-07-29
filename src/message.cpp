
#include "lexer.h"
#include "string.h"
#include "print.h"


/**
 * Because atm we discard the buffers containing all of the read-in source code,
 * we have to reconstruct code segments for linter/warning/error message context.
 *
 * Tokens can do this because they store where they are in the file + how long they are.
 *
 * @TODO support multiple lines of context via an offset, above or below or both.
 * @STATEFUL
 */
const char* reconstruct(u32 line) {
    // find the first token on the requested line...
    u32 i = 0;
    bool found = false;
    for (; i < Lexer::tokenCount; i++) {
        if (Lexer::tokens[i].line == line) {
            found = true;
            break;
        }
    }

    if (!found) { // we didn't find anything on that line...
        return null;
    }

    // begin reconconsituting the tokens on that line as one string
    const char* out = "";

    Token token;
    u32 columnSoFar = 0;
    do {
        u32 j = 1;
        token = Lexer::tokens[i++];

        char* scratch = (char*) pMalloc(sizeof (char) * (token.column + token.length - columnSoFar));

        for (; j < (token.column + token.length - columnSoFar); j++) {
            if (j < (token.column - columnSoFar)) {
                scratch[j - 1] = ' ';

            } else {
                scratch[j - 1] = token.tk[j - token.column + columnSoFar];
            }
        }

        scratch[j - 1] = '\0';

        out = concat(out, scratch);
        free(scratch);

        columnSoFar += token.column + token.length - 1;

    } while (token.line == line && (i < Lexer::tokenCount));

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

