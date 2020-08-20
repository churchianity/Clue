
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
    E_INVALID_CHARACTER,
    W_OVERPRECISE_NUMBER,
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






/**
 * keys correlate directly with MessageEnum (message.h), but our compiler won't help us write good code,
 * so we just have to deal with the error-prone nature of this approach until we think of something better
 *
 * ensure the first element of the array is on a line with it % 10 == 0
 * @TODO replace messages with localization keys (which should probably be the same name as the enum
 */
const MessageId messageIds[] = {
     { MS_LINT, "only digits can come after a digit in an identifier name" },
     { MS_ERROR, "leading zeroes can only be in the form '0x' (hexadecimal), '0b' (binary), '0o' (octal), or '0.' (fractional decimal)" },
     { MS_ERROR, "dot appearing immediately after a number is always invalid" },
     { MS_ERROR, "invalid character" },
     { MS_WARN, "numerics have a maximum precision of 24 characters - extra length is discarded" },
     { MS_WARN, "trying to import file that has already been imported" },
     { MS_ERROR, "trying to import something that isn't a string" },
     { MS_ERROR, "missing operand for unary operator" },
     { MS_ERROR, "missing operand for binary operator" },
     { MS_ERROR, "missing open parentheses" },
     { MS_ERROR, "missing close parentheses" },
     { MS_ERROR, "missing operand for operator" },
     { MS_ERROR, "attempting to add an operand to an operator that is already satisfied" },
     { MS_WARN, "semicolon with nothing before it has no effect" },
     { MS_ERROR, "invalid operator" },
};

#endif

