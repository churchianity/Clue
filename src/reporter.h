
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
    char* message;
    MessageSeverityEnum severity;
    MessageSourceEnum source;
} Message;

extern Message* reporter;


#endif

