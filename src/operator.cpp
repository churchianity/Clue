
#include "table.h"
#include "operator.h"
#include "util.h"


static Table* operatorTable = newTable(50);


void initOperatorTable() {
    Table* t = operatorTable;

    t->insert(t, ":",   NULL);

    t->insert(t, ";",   NULL);
    t->insert(t, ",",   NULL);
    t->insert(t, "{",   NULL);
    t->insert(t, "(",   NULL);
    t->insert(t, ")",   NULL);
    t->insert(t, "}",   NULL);
    t->insert(t, "[",   NULL);
    t->insert(t, "]",   NULL);

    t->insert(t, "=",   NULL);
    t->insert(t, ":=",  NULL);
    t->insert(t, "+=",  NULL);
    t->insert(t, "-=",  NULL);
    t->insert(t, "*=",  NULL);
    t->insert(t, "/=",  NULL);
    t->insert(t, "%=",  NULL);
    t->insert(t, "&=",  NULL);
    t->insert(t, "|=",  NULL);
    t->insert(t, "~=",  NULL);
    t->insert(t, "^=",  NULL);
    t->insert(t, "<<=", NULL);
    t->insert(t, ">>=", NULL);

    t->insert(t, "&&",  NULL);
    t->insert(t, "||",  NULL);

    t->insert(t, "!",   NULL);

    t->insert(t, "!=",  NULL);
    t->insert(t, "==",  NULL);
    t->insert(t, ">=",  NULL);
    t->insert(t, "<=",  NULL);
    t->insert(t, ">",   NULL);
    t->insert(t, "<",   NULL);

    t->insert(t, "+",   NULL);
    t->insert(t, "-",   NULL);
    t->insert(t, "*",   NULL);
    t->insert(t, "/",   NULL);
    t->insert(t, "%",   NULL);
    t->insert(t, "**",  NULL);

    t->insert(t, "&",   NULL);
    t->insert(t, "|",   NULL);
    t->insert(t, "^",   NULL);
    t->insert(t, "~",   NULL);

    t->insert(t, "<<",  NULL);
    t->insert(t, ">>",  NULL);

    t->insert(t, "++",  NULL);
    t->insert(t, "--",  NULL);


    t->insert(t, "//",  NULL);
    t->insert(t, ".",   NULL);
}

u8 precedence(int tt, bool isUnary, bool isPostfix) {
    return 2;
}

