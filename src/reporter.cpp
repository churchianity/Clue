
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"
#include "util.h"


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

