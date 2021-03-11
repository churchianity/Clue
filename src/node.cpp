

#include "node.h"
#include "string.h"
#include "print.h"


// stolen from https://stackoverflow.com/questions/1649027/how-do-i-print-out-a-tree-structure
void prettyPrintTree(ASTNode* node, const char* indent, bool last) {
    if (node == null) {
        print("null\n");
        return;
    }

    print(indent);
    if (last) {
        print("\\-");
        indent = Str :: concat(indent, "  ");

    } else {
        print("|-");
        indent = Str :: concat(indent, "| ");
    }
    print(node);

    if (node->children != null) {
        for (u32 i = 0; i < node->children->length; i++) {
            prettyPrintTree(node->children->data[i], indent, i == (node->children->length - 1));
        }
    }
}

void traverse(ASTNode* node, void (*callback) (ASTNode*)) {
    if (!node) return;

    if (node->children != null) {
        for (u32 i = 0; i < node->children->length; i++) {
            traverse(node->children->data[i], callback);
        }
    }

    callback(node);
}

void print(ASTNode* node) {
    if (node == null) {
        print("null\n");
    }

    if (node->token) {
        if (node->flags != 0) {
            print("%s%s%s\n", ANSI_YELLOW, node->token->tk, ANSI_RESET, node->flags);

        } else {
            print("%s%s%s\n", ANSI_YELLOW, node->token->tk, ANSI_RESET);
        }
    } else {
        print("%s__PROGRAM_ROOT__%s\n", ANSI_YELLOW, ANSI_RESET);
    }
}

