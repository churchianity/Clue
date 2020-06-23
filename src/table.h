
#ifndef TABLE_H
#define TABLE_H

typedef struct TableEntry {
    struct TableEntry* next;
    const char* key;
    void* value;
} TableEntry;

typedef struct Table {
    unsigned int size;
    unsigned int capacity;

    signed int (*insert) (const struct Table* self, const char* key, void* value);
    TableEntry* (*lookup) (const struct Table* self, const char* key);

    TableEntry** entries;
} Table;

Table* newTable(unsigned int capacity);

#endif

