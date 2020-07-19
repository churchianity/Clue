
#include "reporter.h"
#include "token.h"
#include "print.h"
#include "util.h"

static u32 messageCount = 0;
static u32 messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

Message* Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);


void Reporter::add(MessageSeverityEnum severity, MessageSourceEnum source, const char* content) {
    if (messageCapacity <= messageCount) {
        messageCapacity *= 2;

        Reporter::messages = (Message*) pRealloc(Reporter::messages, (sizeof (Message) * messageCapacity));
    }

    Message* message = (Message*) pMalloc(sizeof (Message));

    message->severity = severity;
    message->source = source;
    message->content = content;

    Reporter::messages[messageCount++] = *message;
}


void Reporter::flush() {
    for (u32 i = 0; i < messageCount; i++) {
        print(Reporter::messages[i]);
        free(&Reporter::messages[i]);
    }

    messageCount = 0;
    messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

    Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}

