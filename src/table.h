
#ifndef TABLE_H
#define TABLE_H

typedef struct TableEntry {
    struct TableEntry* next;
    const char* value;
} TableEntry;

typedef struct Table {
    unsigned int capacity;
    TableEntry* entries[];
} Table;

Table* newTable(unsigned int capacity);

#endif

