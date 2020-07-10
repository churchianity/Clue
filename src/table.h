
#ifndef TABLE_H
#define TABLE_H

#include "clue.h"


typedef struct TableEntry {
    TableEntry* next;
    const char* key;
    void* value;
} TableEntry;

typedef struct Table {
    u32 capacity;

    signed int (*insert) (Table* self, const char* key, void* value);
    TableEntry* (*lookup) (const Table* self, const char* key);

    TableEntry** entries;
} Table;

Table* newTable(u32 capacity);

#endif

