
#ifndef MESSAGE_H
#define MESSAGE_H


enum MessageSeverityEnum {
    MS_LINT,
    MS_WARN,
    MS_ERROR
};

struct Message {
    const char* content;
    const char* functionName;
    const char* filename;

    MessageSeverityEnum severity;

    u32 line;
    u32 column;
};

#endif

