

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
    u32 id;
    char* content;
    const char* functionName;
    const char* filename;
    u32 line;
    u32 column;
    MessageSeverityEnum severity;
};

const MessageId messageIds[] = {
    { MS_WARN, "program may be in unsafe state." },
    { MS_ERROR, "weird character with codepoint %hhx" },
};

enum MessageEnum {
    W_PROGRAM_UNSAFE_STATE,
    E_WEIRD_CODEPOINT,
};

#endif

