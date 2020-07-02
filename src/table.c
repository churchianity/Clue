
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strlen

#include "clue.h"
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
        entry = pMalloc(sizeof (TableEntry));
        entry->key = key;
        entry->value = value;

        hashValue = hash(key, self->capacity);
        entry->next = self->entries[hashValue];
        self->entries[hashValue] = entry;

    } else {
        free(entry->value);

        entry->value = value;
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

static void print(const Table* self) {
    printf("tp: %p | capacity: %u | entries:\n", (void*) self, self->capacity);

    for (u32 i = 0; i < self->capacity; ++i) {
        TableEntry* entry = *(self->entries + i);
        printf("%u : %p", i, (void*) entry);

        if (entry) {
            printf(" | %s", entry->key);

            while (entry->next) {
                entry = entry->next;

                printf(" ---> %p | %s", (void*) entry, entry->key);
            }
        }

        printf("\n");
    }
}

Table* newTable(u32 capacity) {
    Table* table = pMalloc(sizeof (Table));

    *table = (Table) {
        .capacity = capacity,
        .insert = &insert,
        .lookup = &lookup,
        .print = &print,
        .entries = pCalloc(capacity, sizeof (TableEntry*))
    };

    return table;
}

