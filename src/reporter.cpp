
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"
#include "util.h"


/**
 * Because atm we discard the buffers containing all of the read-in source code,
 * we have to reconstruct code segments for linter/warning/error message context.
 *
 * Tokens can do this because they store where they are in the file + how long they are.
 *
 * @TODO support multiple lines of context via an offset, above or below or both.
 */
const char* reconstruct(u32 line) {
    // find the first token on the requested line...
    u32 i = 0;
    for (; i < Lexer::tokenCount; i++) {
        if (Lexer::tokens[i].line == line) {
            break;
        }
    }

    if (i == Lexer::tokenCount) { // we didn't find anything on that line...
        return null;
    }

    // begin reconconsituting the tokens on that line as one string
    const char* out = "";

    Token token;
    u32 columnSoFar = 0;
    do {
        u32 j = 1;
        token = Lexer::tokens[i++];

        char* tokenValueAsString = tokenValueToString(&token);

        char* scratch = (char*) pMalloc(sizeof (char) * (token.column + token.length - columnSoFar));

        for (; j < (token.column + token.length - columnSoFar); j++) {
            if (j < (token.column - columnSoFar)) {
                scratch[j - 1] = ' ';

            } else {
                scratch[j - 1] = tokenValueAsString[j - token.column + columnSoFar];
            }
        }

        scratch[j - 1] = '\0';

        out = concat(out, scratch);
        free(scratch);

        columnSoFar += token.column + token.length - 1;

    } while (token.line == line && (i < Lexer::tokenCount));

    return out;
}

static u32 messageCount = 0;
static u32 messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

Message* Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);


/**
 * @STATEFUL
 */
void Reporter :: add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column) {
    if (messageCapacity <= messageCount) {
        messageCapacity *= 2;

        Reporter::messages = (Message*) pRealloc(Reporter::messages, (sizeof (Message) * messageCapacity));
    }

    Message* message = (Message*) pMalloc(sizeof (Message));

    message->severity       = severity;
    message->content        = content;
    message->functionName   = functionName;
    message->filename       = filename;
    message->line           = line;
    message->column         = column;
    message->context        = reconstruct(line);

    Reporter::messages[messageCount++] = *message;
}

void Reporter :: flush() {
    for (u32 i = 0; i < messageCount; i++) {
        print(&Reporter::messages[i]);
    }

    messageCount = 0;
    messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

    Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}






