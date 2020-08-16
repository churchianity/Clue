
#ifndef MESSAGE_H
#define MESSAGE_H


enum MessageSeverityEnum {
    MS_LINT,
    MS_WARN,
    MS_ERROR
};

struct MessageContext {
    const char* functionName;
    const char* filename;
    u32 line;
    u32 column;
};

struct Message {
    MessageContext* ctx;
    u32 id;
};

#endif

