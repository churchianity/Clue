
#include <stdio.h> // stdin
#include <math.h> // pow, remainder, floor, strtod
                  // @TODO replace?

#include "clue.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"
#include "runtime.h"
#include "value.h"

static Table<const char, Value>* global = new Table<const char, Value>();
static ASTNode* program = null;

/*
static inline s32 fToInt(double f) {
    return (s32) floor(f + 0.5);
}

// @TODO doesn't work basically at all
static inline double evalBitwiseNot(ASTNode* node) {
    return ~fToInt(Runtime :: eval(node->children->data[0]).number);
}

static inline u32 evalBitwiseAnd(ASTNode* node) {
    return fToInt(Runtime :: eval(node->children->data[0]).number) & fToInt(Runtime :: eval(node->children->data[1]).number);
}

static inline u32 evalBitwiseOr(ASTNode* node) {
    return fToInt(Runtime :: eval(node->children->data[0]).number) | fToInt(Runtime :: eval(node->children->data[1]).number);
}

static inline u32 evalBitwiseXor(ASTNode* node) {
    return fToInt(Runtime :: eval(node->children->data[0]).number) ^ fToInt(Runtime :: eval(node->children->data[1]).number);
}

static inline u32 evalLeftShift(ASTNode* node) {
    return fToInt(Runtime :: eval(node->children->data[0]).number) << fToInt(Runtime :: eval(node->children->data[1]).number);
}

static inline u32 evalRightShift(ASTNode* node) {
    return fToInt(Runtime :: eval(node->children->data[0]).number) >> fToInt(Runtime :: eval(node->children->data[1]).number);
}

static inline double evalNegation(ASTNode* node) {
    return !Runtime :: eval(node->children->data[0]).number;
}

static inline double evalUnaryPlus(ASTNode* node) {
    return +Runtime :: eval(node->children->data[0]).number;
}

static inline double evalUnaryMinus(ASTNode* node) {
    return -Runtime :: eval(node->children->data[0]).number;
}

static inline double evalBinaryAddition(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number + Runtime :: eval(node->children->data[1]).number;
}

static inline double evalBinarySubtraction(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number - Runtime :: eval(node->children->data[1]).number;
}

static inline double evalMultiplication(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number * Runtime :: eval(node->children->data[1]).number;
}

static inline double evalDivision(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number / Runtime :: eval(node->children->data[1]).number;
}

static inline double evalModulus(ASTNode* node) {
    return remainder(Runtime :: eval(node->children->data[0]).number, Runtime :: eval(node->children->data[1]).number);
}

static inline double evalExponentiation(ASTNode* node) {
    return pow(Runtime :: eval(node->children->data[0]).number, Runtime :: eval(node->children->data[1]).number);
}

static inline bool evalLogicalAnd(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number && Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalLogicalOr(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number || Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalLogicalXor(ASTNode* node) {
    return !Runtime :: eval(node->children->data[0]).number != !Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalGreaterThan(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number > Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalGreaterThanOrEqual(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number >= Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalLessThan(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number < Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalLessThanOrEqual(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number <= Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalEquals(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number == Runtime :: eval(node->children->data[1]).number;
}

static inline bool evalNotEquals(ASTNode* node) {
    return Runtime :: eval(node->children->data[0]).number != Runtime :: eval(node->children->data[1]).number;
}

static inline void evalAssignment(ASTNode* node) {
    Value* valuePointer = (Value*) pMalloc(sizeof (Value));
    Value value = Runtime :: eval(node->children->data[1]);
    valuePointer->type = value.type;

    if (value.type == VT_NUMBER) {
        valuePointer->number = value.number;

    } else if (value.type == VT_STRING) {
        valuePointer->string = value.string;

    } else if (value.type == VT_POINTER) {
        valuePointer->pointer = value.pointer;
    }

    global->insert((node->children->data[0])->token->tk, (node->children->data[0])->token->length, valuePointer);
}

static inline Value evalOperator(ASTNode* node) {
    Value v;
    v.type = VT_STRING;
    v.string = "";

    switch ((s32) node->token->tt) {
        // enumerate the tokens/nodes here that have no effect at runtime.
        case TT_IMPORT:
        case ':':
            break;

        default:
            v.string = "not implemented yet, sorry!\n";
            break;
    }

    return v;
}
*/

Value Runtime_eval(ASTNode* node) {
    Value v;
    v.type = VT_STRING;
    v.string = "";

    // @TODO this probably shouldn't switch on the tokentype, it should switch on the node type - atleast in the case of operators.
    switch (node->token->tt) {
        case TT_SYMBOL:
            v = *global->lookup(node->token->tk, node->token->length)->value;
            break;

        case TT_NUMERIC:
            v.type = VT_NUMBER;
            v.number = strtod(node->token->tk, null); // @TODO remove strtod
            break;

        case TT_STRING:
            v.type = VT_STRING;
            v.string = node->token->tk;
            break;

        default:
            // v = evalOperator(node);
            break;
    }

    return v;
}

void Runtime_printProgramTree(ASTNode* program) {
    if (!program) {
        print("No program.");
    }

    for (u32 i = 0; i < program->children->length; i++) {
        prettyPrintTree(program->children->data[i]);
    }
}

void Runtime_doIt(char* buffer, const char* filename) {
    Array<Token>* tokens = Lexer_tokenize(buffer, filename);
    program = Parser_parse(tokens);
    Runtime_printProgramTree(program);
}

Table<const char, Value>* Runtime_getGlobalSymbolTable() {
    return global;
}

// @TODO
static void deleteEverything() {
    Lexer_files->clear([] (TableEntry<const char, void>* entry) {
        pFreeConst(entry->key);
    });
}

void Runtime_help() {
    print(
        "This is the Clue interpreter. It parses Clue source code line-by-line, incrementally.\n"
        "\n"
        "Lines containing exactly one character can have special meaning:\n"
        "\t'.' - exit the interpreter\n"
        "\t'/' - destroy the program as it currently exists, but keep running the interpreter\n"
        "\t'#' - print the state of the lexer\n"
        "\t'*' - print the abstract syntax tree\n"
        "\t'?' - show this help text\n"
    );
}

void Runtime_interactive() {
    #define CLUE_SANDBOX_MODE_MAX_LINE_LENGTH 160
    char s[CLUE_SANDBOX_MODE_MAX_LINE_LENGTH];

    print(
        "Clue line-by-line interpreter, version %s\n"
        "For help, consult the docs, or type '?' and hit enter.\n"
        , __CLUE_VERSION_NUMBER__
    );

    u32 line = 1;
    do {
        print(">>> ");

        if (fgets(s, CLUE_SANDBOX_MODE_MAX_LINE_LENGTH, stdin) == null) {
            die("error reading line and storing it here: %p\n", (void*) s);
        }

        if (Str_len(s) == 2) {
            switch (s[0]) {
                case '.':
                    print("Bye!\n");
                    return;

                case '/':
                    print("` deleting everything...\n");
                    deleteEverything();
                    continue;

                case '#':
                    print("` printing the state of the lexer...\n");
                    Lexer_print();
                    continue;

                case '*':
                    print("` printing program tree...\n");
                    Runtime_printProgramTree(program);
                    continue;

                case '?':
                    Runtime_help();
                    continue;
            }
        }

        Lexer_tokenize(s, "stdin", line);
        program = Parser_parse(Lexer_tokens);

        line++;

    } while (true);

    #undef CLUE_SANDBOX_MODE_MAX_LINE_LENGTH
}

