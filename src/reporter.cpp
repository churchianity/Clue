
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"
#include "util.h"


static Array<Message>* messages = new Array<Message>(CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);

/**
 * keys correlate directly with MessageEnum (message.h), but c++ doesn't let you use designated initializers
 * so we just have to deal with the error-prone nature of this approach until we think of something better
 *
 * ensure the first element of the array is on a line with it % 10 == 0
 * @TODO replace messages with localization keys (which should probably be the same name as the enum
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-extensions"
static const MessageId messageIds[] = {
    [L_FOLLOWING_DIGIT_IN_IDENTIFIER]       = { MS_LINT, "only digits can come after a digit in an identifier name" },
    [E_BAD_LEADING_ZERO]                    = { MS_ERROR, "leading zeroes can only be in the form '0x' (hexadecimal), '0b' (binary), '0o' (octal), or '0.' (fractional decimal)" },
    [E_MULTIPLE_DOTS_IN_NUMBER]             = { MS_ERROR, "dot appearing immediately after a number is always invalid" },
    [E_INVALID_CHARACTER]                   = { MS_ERROR, "invalid character" },
    [W_OVERPRECISE_NUMBER]                  = { MS_WARN, "numerics have a maximum precision of 24 characters - extra length is discarded" },
    [W_DUPLICATE_IMPORT]                    = { MS_WARN, "trying to import file that has already been imported" },
    [E_BAD_IMPORT]                          = { MS_ERROR, "trying to import something that isn't a string" },
    [E_MISSING_OPERAND_FOR_UNARY_OPERATOR]  = { MS_ERROR, "missing operand for unary operator" },
    [E_MISSING_OPERAND_FOR_BINARY_OPERATOR] = { MS_ERROR, "missing operand for binary operator" },
    [E_MISSING_OPEN_PAREN]                  = { MS_ERROR, "missing open parentheses" },
    [E_MISSING_CLOSE_PAREN]                 = { MS_ERROR, "missing close parentheses" },
    [E_MISSING_OPERAND_FOR_OPERATOR]        = { MS_ERROR, "missing operand for operator" },
    [E_TOO_MANY_OPERANDS]                   = { MS_ERROR, "attempting to add an operand to an operator that is already satisfied" },
    [W_USELESS_SEMICOLON]                   = { MS_WARN, "semicolon with nothing before it has no effect" },
    [E_INVALID_OPERATOR]                    = { MS_ERROR, "invalid operator" },
};
#pragma GCC diagnostic pop

static inline const char* messageSeverityToColor(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return ANSI_BLUE;
        case MS_WARN: return ANSI_YELLOW;
        case MS_ERROR: return ANSI_RED;
        default: die("bad message severity\n"); return null;
    }
}

static inline const char* messageSeverityToString(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return "lint";
        case MS_WARN: return "warn";
        case MS_ERROR: return "error";
        default: die("bad message sevevity\n"); return null;
    }
}

/**
 * @TODO i should just make a memset() like thingy in string.h
 */
static inline char* fillWithSpaces(u32 length) {
    char* buffer = (char*) pMalloc(sizeof (char) * length + 1);

    u32 i = 0;
    for (; i < length; i++) {
        buffer[i] = ' ';
    }

    buffer[i] = '\0';

    return buffer;
}

static inline char* makePointyThing(u32 column) {
    char* buffer = fillWithSpaces(column);

    buffer[column - 1] = '^';

    return buffer;
}

/**
 * Because atm we free the buffers containing all of the read-in source code,
 * we have to reconstruct code segments for linter/warning/error message context.
 *
 * Tokens can do this because they store where they are in the file + how long they are.
 *
 * @TODO support multiple lines of context via an offset, above or below or both.
 * @STATEFUL
 */
static inline const char* reconstruct(const char* filename, u32 line) {
    // find the first token in the request file, on the requested line
    const Token* token = null;

    u32 columnSoFar = 0; // prev token's length + prev amount of leading whitespace
    u32 amountOfLeadingWhitespace = 0;

    const char* out = "";

    u32 i = 0;
    for (; i < Lexer::tokens->size(); i++) {
        if (streq(Lexer::tokens->data[i]->filename, filename) && (Lexer::tokens->data[i]->line == line)) {
            token = Lexer::tokens->data[i];

            // the -1 is because column counts are 1-indexed
            amountOfLeadingWhitespace = token->column - columnSoFar - 1;
            columnSoFar += token->length + amountOfLeadingWhitespace;

            char* leadingWhitespace = fillWithSpaces(amountOfLeadingWhitespace);

            out = concat(3, out, leadingWhitespace, token->tk);
            free(leadingWhitespace);
        }
    }

    return out;
}

/**
 *  Example printed message:
 *
 *  error: missing right-hand operand for operator
 *  in function 'funcName': ./../src/foo.clue:14:51
 *          Int x := 4 !;
 *                     ^
 */
static void print(const Message* message) {
    const char* fn = message->functionName;
    char* pointyThing = makePointyThing(message->column);

    // i'm so sorry.
    print("\n    %s%s%s: %s\n    %s%s%s%s:%u:%u\n    %s\n    %s%s%s\n"
           , messageSeverityToColor(message->severity), messageSeverityToString(message->severity), ANSI_RESET
           , message->content
           , fn ? "in function '" : "", fn ? fn : "", fn ? "': " : ""
           , message->filename, message->line, message->column
           , reconstruct(message->filename, message->line)
           , ANSI_RED, pointyThing, ANSI_RESET
    );

    free(pointyThing);
}

void Reporter :: add(u32 id, const char* functionName, const char* filename, u32 line, u32 column) {
    Message* message = (Message*) pMalloc(sizeof (Message));

    MessageId messageId = messageIds[id];

    message->severity     = messageId.severity;
    message->content      = messageId.content;

    message->functionName = functionName;
    message->filename     = filename;
    message->line         = line;
    message->column       = column;

    messages->push(message);
}

void Reporter :: add(u32 id, ASTNode* node) {
    add(id, null, node->token->filename, node->token->line, node->token->column);
}

void Reporter :: report(u32 id, const char* functionName, const char* filename, u32 line, u32 column) {
    add(id, null, filename, line, column);

    Reporter::flush();
}

void Reporter :: report(u32 id, ASTNode* node) {
    report(id, null, node->token->filename, node->token->line, node->token->column);
}

/**
 * @STATEFUL
 */
void Reporter :: flush() {
    messages->forEach(
        [] (Message* message) {
            print(message);
            free(message);
        }
    );

    delete messages;
    messages = new Array<Message>(CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}

