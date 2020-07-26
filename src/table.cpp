
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"
#include "string.h"
#include "table.h"
#include "util.h"


static u32 hash(const char* value, u32 capacity) {
    u32 hash = 0;

    while (*value) {
        hash = hash * 31 + *value++;
    }

    return hash % capacity;
}

signed int Table :: insert(const char* key, void* value) {
    TableEntry* entry = lookup(key);

    u32 hashValue;

    if (!entry) {
        entry = (TableEntry*) pMalloc(sizeof (TableEntry));
        entry->key = key;
        entry->value = value;

        hashValue = hash(key, capacity);
        entry->next = entries[hashValue];
        entries[hashValue] = entry;

        return 0;

    } else {
        free(entry->value);

        entry->value = value;
        return 1;
    }
}

TableEntry* Table :: lookup(const char* key) {
    TableEntry* entry = entries[hash(key, capacity)];

    for (; entry != null; entry = entry->next) {
        if (streq(key, entry->key)) {
            return entry;
        }
    }

    return null;
}

void Table :: clear() {
    for (u32 i = 0; i < capacity; i++) {
        TableEntry* entry = entries[i];
        TableEntry* prev;

        while (entry) {
            prev = entry;
            free(prev);
            entry = entry->next;
        }
    }

    entries = (TableEntry**) pCalloc(capacity, sizeof (TableEntry*));
}

Table* newTable(u32 capacity) {
    Table* table = (Table*) pMalloc(sizeof (Table));

    table->capacity = capacity;
    table->entries = (TableEntry**) pCalloc(capacity, sizeof (TableEntry*));

    return table;
}

