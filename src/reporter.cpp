
#include "reporter.h"
#include "print.h"
#include "util.h"

static u32 messageCount = 0;
static u32 messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

Message* Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);


void Reporter::add(Message message) {
    if (messageCapacity <= messageCount) {
        messageCapacity *= 2;

        Reporter::messages = (Message*) pRealloc(Reporter::messages, (sizeof (Message) * messageCapacity));
    }

    Reporter::messages[messageCount++] = message;
}


void Reporter::flush() {
    for (u32 i = 0; i < messageCount; i++) {
        print(Reporter::messages[i]);
    }
}

