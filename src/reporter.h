
#ifndef REPORTER_H
#define REPORTER_H


typedef enum {
    MS_LINT,
    MS_WARNING,
    MS_ERROR
} MessageSeverityEnum;

typedef enum {
    MC_LEXER,
    MC_PARSER
} MessageSourceEnum;

typedef struct {
    const char* content;
    MessageSeverityEnum severity;
    MessageSourceEnum source;
} Message;

namespace Reporter {
    extern Message* messages;

    extern void flush();
    extern void add(Message message);
}

#endif

