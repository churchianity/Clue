
#ifndef TABLE_H
#define TABLE_H

#include "clue.h"


typedef struct TableEntry {
    struct TableEntry* next;
    const char* key;
    void* value;
} TableEntry;

typedef struct Table {
    u32 capacity;

    signed int (*insert) (struct Table* self, const char* key, void* value);
    TableEntry* (*lookup) (const struct Table* self, const char* key);
    void (*print) (const struct Table* self);

    TableEntry** entries;
} Table;

Table* newTable(u32 capacity);

#endif

