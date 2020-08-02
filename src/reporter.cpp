
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"
#include "util.h"


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

/**
 * @TODO i should just make a memset() like thingy in string.h
 */
static inline char* fillWithSpaces(s32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = ' ';
    }

    buffer[i] = '\0';

    return buffer;
}

static inline char* makePointyThing(u32 column) {
    char* buffer = fillWithSpaces(column);

    buffer[column - 1] = '^';

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
static inline const char* reconstruct(const char* filename, u32 line) {
    // find the first token in the request file, on the requested line
    const Token* token = null;

    s32 columnSoFar = 0;
    s32 amountOfLeadingWhitespace = 0;

    const char* out = "";

    s32 i = 0;
    for (; i < Lexer::tokenCount; i++) {

        if (streq(Lexer::tokens[i].filename, filename) && (Lexer::tokens[i].line == line)) {
            token = Lexer::tokens + i;

            // the -1 is because column counts are 1-indexed
            amountOfLeadingWhitespace += (token->column - 1) - columnSoFar;
            columnSoFar += (token->column - 1) + token->length;

            char* leadingWhitespace = fillWithSpaces(amountOfLeadingWhitespace);

            out = concat(3, out, leadingWhitespace, token->tk);
            free(leadingWhitespace);
        }
    }

    return out;
}

/**
 *  Example printed message:
 *
 *  error: missing right-hand operand for operator
 *  in function 'funcName': ./../src/foo.clue:14:51
 *          Int x := 4 !;
 *                     ^
 */
static void print(const Message* message) {
    const char* fn = message->functionName;
    char* pointyThing = makePointyThing(message->column);

    // i'm so sorry.
    printf("\n    %s%s%s: %s\n    %s%s%s%s:%u:%u\n    %s\n    %s%s%s\n"
           , messageSeverityToColor(message->severity), messageSeverityToString(message->severity), ANSI_RESET
           , message->content
           , fn ? "in function '" : "", fn ? fn : "", fn ? "': " : ""
           , message->filename, message->line, message->column
           , reconstruct(message->filename, message->line)
           , ANSI_RED, pointyThing, ANSI_RESET
    );

    free(pointyThing);
}

u32 Reporter::messageCount = 0;
u32 Reporter::messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

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

    message->content        = content;
    message->functionName   = functionName;
    message->filename       = filename;
    message->severity       = severity;
    message->line           = line;
    message->column         = column;

    Reporter::messages[messageCount++] = *message;
}

/**
 * Immediately constructs a message on the stack and prints it without ever storing it on the heap.
 * These should probably all be fatal errors... we currently exit after reporting.
 */
void Reporter :: report(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column) {
    Message message = {
        content,
        functionName,
        filename,
        severity,
        line,
        column
    };

    print(&message);
    exit(1);
}

/**
 * @STATEFUL
 */
void Reporter :: flush() {
    for (u32 i = 0; i < messageCount; i++) {
        print(Reporter::messages + i);
        free(Reporter::messages + i);
    }

    messageCount = 0;
    messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

    Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}

