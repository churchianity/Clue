
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "table.h"
#include "util.h"

static unsigned int hash(const char* value, unsigned int capacity) {
    unsigned int hash = 0;

    while (*value) {
        hash = hash * 31 + *value++;
    }

    return hash % capacity;
}

static signed int insert(Table* self, const char* key, void* value) {
    TableEntry* entry = self->lookup(self, key);

    unsigned int hashValue;

    if (!entry) {
        entry = pmalloc(sizeof (TableEntry));
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
        if (strcmp(key, entry->key) == 0) {
            return entry;
        }
    }

    return NULL;
}

static void print(const Table* self) {
    printf("%p | capacity: %d | entries:\n", (void*) self, self->capacity);

    for (unsigned int i = 0; i < self->capacity; ++i) {
        TableEntry* entry = *(self->entries + i);
        printf("%d : %p", i, (void*) entry);

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

/**
 *
 */
Table* newTable(unsigned int capacity) {
    Table* table = pmalloc(sizeof (Table));

    *table = (Table) {
        .capacity = capacity,
        .insert = &insert,
        .lookup = &lookup,
        .print = &print,
        .entries = pcalloc(capacity, sizeof (TableEntry*))
    };

    return table;
}

