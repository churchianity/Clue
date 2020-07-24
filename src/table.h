
#ifndef TABLE_H
#define TABLE_H

#include "clue.h"


// @TODO we should try and support keys of any type
struct TableEntry {
    TableEntry* next;
    const char* key;
    void* value;
};

struct Table {
    u32 capacity;

    signed int (*insert) (Table* self, const char* key, void* value);
    TableEntry* (*lookup) (const Table* self, const char* key);

    TableEntry** entries;
};

extern Table* newTable(u32 capacity);

#endif

