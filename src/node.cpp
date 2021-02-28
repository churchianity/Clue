

#include "node.h"
#include "string.h"
#include "print.h"


// stolen from https://stackoverflow.com/questions/1649027/how-do-i-print-out-a-tree-structure
void prettyPrintTree(ASTNode* node, const char* indent, bool last) {
    print(indent);
    if (last) {
        print("\\-");
        indent = Str :: concat(indent, "  ");

    } else {
        print("|-");
        indent = Str :: concat(indent, "| ");
    }
    print("&ASTNode: %p, f: %d, tk: %s%s%s\n", (void*) node, node->flags, ANSI_YELLOW, node->token->tk, ANSI_RESET);

    if (node->children != null) {
        for (u32 i = 0; i < node->children->length; i++) {
            prettyPrintTree(node->children->data[i], indent, i == (node->children->length - 1));
        }
    }
}

void traverse(ASTNode* node, void (*callback) (ASTNode*)) {
    if (!node) {
        return;
    }

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

    print("&ASTNode %p | flags: %d | tk: %s\n", (void*) node, node->flags, node->token->tk);
}

