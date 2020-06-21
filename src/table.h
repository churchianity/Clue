
#ifndef TABLE_H
#define TABLE_H

typedef struct TableEntry {
    TableEntry* next;
    char* key;
    char* value;
} TableEntry;

void install(char* value);
TableEntry* lookup(char* value);

#endif

