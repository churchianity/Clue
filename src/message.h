
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

/**
 * Keep the first element of this enum on a line where it % 10 == 0.
 */
enum MessageEnum {
    L_FOLLOWING_DIGIT_IN_IDENTIFIER,
    E_BAD_LEADING_ZERO,
    E_MULTIPLE_DOTS_IN_NUMBER,
    W_OVERPRECISE_NUMBER,
    E_INVALID_CHARACTER,
    W_DUPLICATE_IMPORT,
    E_BAD_IMPORT,
    E_MISSING_OPERAND_FOR_UNARY_OPERATOR,
    E_MISSING_OPERAND_FOR_BINARY_OPERATOR,
    E_MISSING_OPEN_PAREN,
    E_MISSING_CLOSE_PAREN,
    E_MISSING_OPERAND_FOR_OPERATOR,
    E_TOO_MANY_OPERANDS,
    W_USELESS_SEMICOLON,
    E_INVALID_OPERATOR,
};

#endif

