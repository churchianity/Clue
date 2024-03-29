
#include <stdio.h> // @TODO snprintf -- needs replacing

#include "clue.h"
#include "lexer.h"
#include "print.h"
#include "string.h"
#include "reporter.h"
#include "runtime.h"
#include "token.h"


static Array<Message>* messages = new Array<Message>();

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
    for (; i < Lexer_tokens->length; i++) {
        if (Str_eq(Lexer_tokens->data[i]->filename, filename) && (Lexer_tokens->data[i]->line == line)) {
            token = Lexer_tokens->data[i];

            // the -1 is because column counts are 1-indexed
            amountOfLeadingWhitespace = token->column - columnSoFar - 1;
            columnSoFar += token->length + amountOfLeadingWhitespace;

            char* leadingWhitespace = fillWithSpaces(amountOfLeadingWhitespace);

            out = Str_concat(3, out, leadingWhitespace, token->tk);
            pFree(leadingWhitespace);
        }
    }

    return out;
}

// @STATEFUL
void Reporter_rebuild(const char* filename) {
    u32 i = 0;
    u32 line = 1;

    while (i < Lexer_tokens->length) {
        if (Str_eq(Lexer_tokens->data[i]->filename, filename)) {
            line = Lexer_tokens->data[i]->line;

            do {
                i++;

                if ((i < Lexer_tokens->length) || ((Lexer_tokens->data[i]->line != line) || Str_eq(Lexer_tokens->data[i]->filename, filename))) {
                    i++;
                    break;
                }

            } while ((i < Lexer_tokens->length) || Str_eq(Lexer_tokens->data[i]->filename, filename));

            print(reconstruct(filename, line));
            print("%d\n", i);
        }

        i++;
    }
}

// rarely, but sometimes you might want to print a message without any language entity context.
static void printById(u32 messageId) {
    MessageId mId = messageIds[messageId];

    print("\n    %s%s%s: %s\n\n"
          , messageSeverityToColor(mId.severity), messageSeverityToString(mId.severity), ANSI_RESET, mId.content);
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
    const char* functionLabel = fn ? "in function '" : "";

    // i'm so sorry.
    print("\n    %s%s%s: %s\n    %s%s%s%s:%u:%u\n    %s\n    %s%s%s\n"
          , messageSeverityToColor(message->severity), messageSeverityToString(message->severity), ANSI_RESET
          , message->content
          , functionLabel, fn ? fn : "", fn ? "': " : ""
          , message->filename, message->line, message->column
          , reconstruct(message->filename, message->line)
          , ANSI_RED, pointyThing, ANSI_RESET
    );

    pFree(pointyThing);
}

void Reporter_flush() {
    messages->forEach(
        [] (Message* message) {
            print(message);
            pFree(message->content);
            pFree(message);
        }
    );

    delete messages;
    messages = new Array<Message>();
}

void Reporter_add(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...) {
    va_list args;
    va_start(args, column);

    Message* message = (Message*) pMalloc(sizeof (Message));

    MessageId messageId   = messageIds[id];
    message->id           = id;
    message->severity     = messageId.severity;

    // make the content string with the formatted varargs.
    // @TODO fix this is all completely fucked
    const u32 padding = 256; // @NOTE fix
    u32 contentLength = Str_len(messageId.content) + padding;
    char* content = (char*) pMalloc(sizeof (char) * contentLength);
    vsnprintf(content, contentLength, messageId.content, args); // @TODO replace vsnprintf.

    message->content      = content;

    message->functionName = functionName;
    message->filename     = filename;
    message->line         = line;
    message->column       = column;

    messages->push(message);
    va_end(args);
}

void Reporter_add(u32 id, const char* functionName, Token* token, ...) {
    va_list args;
    va_start(args, token);

    Reporter_add(id, functionName, token->filename, token->line, token->column, args);
    va_end(args);
}

void Reporter_add(u32 id, ASTNode* node, ...) {
    va_list args;
    va_start(args, node);

    // @TODO replace null with node->closure->name if it exists
    Reporter_add(id, null, node->token, args);
    va_end(args);
}

void Reporter_report(u32 id, const char* functionName, const char* filename, u32 line, u32 column, ...) {
    static bool safe = true;

    va_list args;
    va_start(args, column);

    Reporter_add(id, functionName, filename, line, column, args);
    Reporter_flush();
    va_end(args);

    // when we 'report' normally, this should exit the program.
    // However, when debugging we might want to see more.
    // this will frequently case segfaults and other ungraceful bad things to happen, but it's better to see them than hide them,
    // and more important than that - to see when supposedly 'fatal' errors can actually not be fatal
    #if CLUE_DEBUG_LEVEL <= 0
    exit(1);
    #else
    // only print warning if we haven't done so yet.
    if (safe) {
        printById(W_PROGRAM_UNSAFE_STATE);
        safe = false;
    }
    #endif
}

void Reporter_report(u32 id, const char* functionName, Token* token, ...) {
    va_list args;
    va_start(args, token);

    Reporter_report(id, functionName, token->filename, token->line, token->column, args);
    va_end(args);
}

void Reporter_report(u32 id, ASTNode* node, ...) {
    va_list args;
    va_start(args, node);

    // @TODO replace null with node->closure->name if it exists
    Reporter_report(id, null, node->token, args);
    va_end(args);
}

