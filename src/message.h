

#ifndef MESSAGE_H
#define MESSAGE_H


enum MessageSeverityEnum {
    MS_LINT,
    MS_WARN,
    MS_ERROR
};

struct MessageId {
    MessageSeverityEnum severity;
    const char* content;
};

struct Message {
    const char* content;
    const char* functionName;
    const char* filename;
    u32 line;
    u32 column;
    MessageSeverityEnum severity;
};

const MessageId messageIds[] = {
    { MS_WARN, "program may be in unsafe state" },
};

enum MessageEnum {
    W_PROGRAM_UNSAFE_STATE,
};

#endif

