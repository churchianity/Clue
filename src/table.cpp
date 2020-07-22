
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

static signed int insert(Table* self, const char* key, void* value) {
    TableEntry* entry = self->lookup(self, key);

    u32 hashValue;

    if (!entry) {
        entry = (TableEntry*) pMalloc(sizeof (TableEntry));
        entry->key = key;
        entry->value = value;

        hashValue = hash(key, self->capacity);
        entry->next = self->entries[hashValue];
        self->entries[hashValue] = entry;

    } else {
        free(entry->value);

        entry->value = value;
        return 1;
    }

    return 0;
}

static TableEntry* lookup(const Table* self, const char* key) {
    TableEntry* entry = self->entries[hash(key, self->capacity)];

    for (; entry != NULL; entry = entry->next) {
        if (streq(key, entry->key)) {
            return entry;
        }
    }

    return NULL;
}

Table* newTable(u32 capacity) {
    Table* table = (Table*) pMalloc(sizeof (Table));

    table->capacity = capacity;
    table->insert = &insert;
    table->lookup = &lookup;

    table->entries = (TableEntry**) pCalloc(capacity, sizeof (TableEntry*));

    return table;
}

