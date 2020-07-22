
#include "print.h"
#include "reporter.h"
#include "token.h"
#include "util.h"

static u32 messageCount = 0;
static u32 messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

Message* Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);

// base assumption about messages in the compiler:
//      - we are always going to care about some filename, line number, and column number
//      - we are going to always want some other textual content
//      - if the code is in a function, we also want to know what the name of the function is
//          - @NOTE for overloaded functions, do we want to print the full function signature? NO
//
//
//      lint: alphabetical characters can't follow digits in identifier names
//      in function 'funcName': ./baz.clue:124:10
//              Int x2n;
//                    ^
//
//      warn: unused variable 'x'
//      in function 'doSomeStuff': ./bar.clue:10:10
//              Int x;
//                  ^
//
//      error: missing right-hand operand for operator
//      in function 'funcName': ./../src/foo.clue:14:51
//              Int x := 4 !;
//                         ^
//
//      error: missing right-hand operand for operator
//      ./../src/foo.clue:14:51
//              Int x := 4 !;
//                         ^
//
void Reporter::add(MessageSeverityEnum severity, const char* content, const char* functionName, const char* filename, u32 line, u32 column) {
    if (messageCapacity <= messageCount) {
        messageCapacity *= 2;

        Reporter::messages = (Message*) pRealloc(Reporter::messages, (sizeof (Message) * messageCapacity));
    }

    Message* message = (Message*) pMalloc(sizeof (Message));

    message->severity = severity;
    message->content = content;
    message->functionName = functionName;
    message->filename = filename;
    message->line = line;
    message->column = column;

    Reporter::messages[messageCount++] = *message;
}

void Reporter::flush() {
    for (u32 i = 0; i < messageCount; i++) {

    }

    messageCount = 0;
    messageCapacity = CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY;

    Reporter::messages = (Message*) pMalloc(sizeof (Message) * CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}

