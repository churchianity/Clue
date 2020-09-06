
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "token.h"
#include "trace.h"


// @TODO move me.
static inline const char* messageSeverityToColor(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return ANSI_BLUE;
        case MS_WARN: return ANSI_YELLOW;
        case MS_ERROR: return ANSI_RED;
        default: die("bad message severity\n"); return null;
    }
}

// @TODO move me.
static inline const char* messageSeverityToString(MessageSeverityEnum severity) {
    switch (severity) {
        case MS_LINT: return "lint";
        case MS_WARN: return "warn";
        case MS_ERROR: return "error";
        default: die("bad message sevevity\n"); return null;
    }
}


static Array<Message>* messages = new Array<Message>();


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
 * @TODO support multiple lines of context via an offset? (above or below or both?)
 * @STATEFUL - needs the lexer state (just the tokens array, really)
 */
static inline const char* reconstruct(const char* filename, u32 line) {
    // find the first token in the request file, on the requested line
    const Token* token = null;

    u32 columnSoFar = 0; // prev token's length + prev amount of leading whitespace
    u32 amountOfLeadingWhitespace = 0;

    const char* out = "";

    u32 i = 0;
    for (; i < Lexer::tokens->length; i++) {
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
 *  Example printed messages:
 *
 *  error: missing right-hand operand for operator
 *  in function 'funcName': ./../src/foo.clue:14:51
 *          Int x := 4 !;
 *                     ^
 *
 *  lint: only digits can follow a digit in an identifier name
 *  ./../src/subfolder/bar.clue:2109:7
 *      y2k : String = "message example";
 *        ^
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

void Reporter :: flush() {
    messages->forEach(
        [] (Message* message) {
            print(message);
        }
    );

    delete messages;
    messages = new Array<Message>();
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
    exit(1);
}

void Reporter :: report(u32 id, ASTNode* node) {
    report(id, null, node->token->filename, node->token->line, node->token->column);
}

