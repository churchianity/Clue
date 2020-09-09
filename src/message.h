
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
 * keys correlate directly with MessageEnum (message.h), but our compiler won't help us write good code,
 * so we just have to deal with the error-prone nature of this approach until we think of something better
 *
 * do NOT add anything to this list unless you also add an appropriate enum entry below as well, and vice-versa
 *
 * @TODO replace messages with localization keys (which should probably be the same name as the enum
 */
const MessageId messageIds[] = {
     { MS_LINT, "only digits can come after a digit in an identifier name" },
     { MS_ERROR, "leading zeroes can only be in the form '0x' (hexadecimal), '0b' (binary), '0o' (octal), '0.' (fractional decimal), or just '0'" },
     { MS_ERROR, "dot appearing immediately after a number is always invalid" }, //@TODO this is a case of we know something is wrong & we don't know why
     { MS_ERROR, "invalid character - did you paste some weird whitespace characters in?" },
     { MS_WARN, "numerics have a maximum precision of 24 characters - extra length is discarded" },
     { MS_WARN, "trying to import file that has already been imported" },
     { MS_ERROR, "trying to import something that isn't a valid string" },
     { MS_ERROR, "missing operand for unary operator" },
     { MS_ERROR, "missing operand for binary operator" },
     { MS_ERROR, "missing open parentheses" },
     { MS_ERROR, "missing close parentheses" },
     { MS_ERROR, "missing operand for operator" },
     { MS_ERROR, "attempting to add an operand to an operator that is already satisfied" },
     { MS_WARN, "semicolon has no effect" },
     { MS_ERROR, "invalid operator" },
     { MS_ERROR, "no matching quotemark" },
     { MS_LINT, "an identifier name shouldn't be this long." },
     { MS_ERROR, "consider storing data of this size somewhere else." },
     { MS_ERROR, "leftover operand" },
     { MS_ERROR, "expecting a binary operator here. what's the previous token doing?" },
     { MS_ERROR, "couldn't open the file - is the path correct?" },
};

/**
 * do NOT add anything to this list unless you also add an appropriate entry above as well, and vice-versa
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
    E_NO_CLOSING_QUOTEMARK,
    L_LONG_SYMBOL,
    E_LONG_STRING,
    E_LEFTOVER_OPERAND,
    E_EXPECTING_BINARY_OPERATOR,
    E_COULDNT_GET_FILE_HANDLE
};

#endif

