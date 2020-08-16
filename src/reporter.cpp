
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"
#include "util.h"


static Array<Message>* messages = new Array<Message>(CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);










struct MessageId {
    MessageSeverityEnum severity;
    const char* content;
};

// all of the contents of all of the messages the reporter can send.
// keep the first element on a line number where it % 10 == 0.
static MessageId messages[] = {
    { MS_LINT, "only alphabetical characters can follow a digit in an identifier name" },
    { MS_ERROR, "leading zeroes can only be in the form '0x' (hexadecimal), '0b' (binary), 0o' (octal), or '0.' (fractional decimal)" },
    { MS_ERROR, "dot appearing immediately after a number is always invalid" },
    { MS_WARN, "numerics have a maximum precision of 24 characters - extra length is discarded" },
    { MS_ERROR, "invalid character" },
    { MS_WARN, "trying to import file that has already been imported" },
    { MS_ERROR, "trying to import something that isn't a string" },
    { MS_ERROR, "missing operand for unary operator" },
    { MS_ERROR, "missing operand for binary operator" },
    { MS_ERROR, "Missing open parentheses" },
    { MS_ERROR, "no matching close paren" },
    { MS_ERROR, "missing operand for operator" },
    { MS_ERROR, "attempting to add an operand to an operator that is already satisfied" },
    { MS_WARN, "semicolon with nothing before it has no effect" },
    { MS_ERROR, "invalid operator" },
};

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
    const char* fn = message->ctx->functionName;
    char* pointyThing = makePointyThing(message->ctx->column);

    // i'm so sorry.
    print("\n    %s%s%s: %s\n    %s%s%s%s:%u:%u\n    %s\n    %s%s%s\n"
           , messageSeverityToColor(messages[message->id].severity), messageSeverityToString(messages[message->id].severity), ANSI_RESET
           , messages[message->id].content
           , fn ? "in function '" : "", fn ? fn : "", fn ? "': " : ""
           , message->ctx->filename, message->ctx->line, message->ctx->column
           , reconstruct(message->ctx->filename, message->ctx->line)
           , ANSI_RED, pointyThing, ANSI_RESET
    );

    free(pointyThing);
}

static MessageContext* nodeToContext(ASTNode* node) {
    MessageContext* ctx = (MessageContext*) pMalloc(sizeof (MessageContext));

    ctx->functionName   = null;
    ctx->filename       = node->token->filename;
    ctx->line           = node->token->line;
    ctx->column         = node->token->column;

    return ctx;
}

void add(u32 id, ASTNode* node) {
    Message* message = (Message*) pMalloc(sizeof (Message));

    message->ctx = nodeToContext(node);
    message->id = id;

    Reporter::messages->push(message);
}

void add(u32 id, const char* functionName, const char* filename, u32 line, u32 column) {
    Message* message = (Message*) pMalloc(sizeof (Message));
}

void report(u32 id, ASTNode* node) {

}

void report(u32 id, MessageContext context) {

}

/**
 * @STATEFUL
 */
void Reporter :: flush() {
    Reporter::messages->forEach(
        [] (Message* message) {
            print(message);
            free(message);
        }
    );

    delete Reporter::messages;
    Reporter::messages = new Array<Message>(CLUE_INITIAL_MESSAGE_ARRAY_CAPACITY);
}

