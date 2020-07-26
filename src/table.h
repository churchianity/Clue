
#ifndef TABLE_H
#define TABLE_H

#include "types.h"


// @TODO we should try and support keys of any type
struct TableEntry {
    TableEntry* next;
    const char* key;
    void* value;
};

struct Table {
    u32 capacity;

    signed int insert(const char* key, void* value);
    TableEntry* lookup(const char* key);
    void clear();

    TableEntry** entries;
};

extern Table* newTable(u32 capacity);

#endif

