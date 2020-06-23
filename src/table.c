
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "table.h"
#include "util.h"

/**
 *
 */
static unsigned int hash(const char* value, unsigned int capacity /*, unsigned int seed = 0*/ ) {
    unsigned int hash = 0; // seed

    while (*value) {
        hash = hash * 101 + *value++;
    }

    return hash % capacity;
}

/**
 * @TODO i think there's a segfault bug with the overwrite case
 */
static signed int insert(const Table* self, const char* key, void* value) {
    TableEntry* entry = self->lookup(self, key);

    if (!entry) {
        entry = pmalloc(sizeof (TableEntry));
        entry->key = key;
        entry->value = value;
        entry->next = NULL;

        self->entries[hash(value, self->capacity)] = entry;

        return 0;

    } else {
        free(entry->value);

        entry->value = strdup(value);

        if (!entry->value) {
            return -2;
        }
    }

    return -1;
}

/**
 *
 */
static TableEntry* lookup(const Table* self, const char* key) {
    TableEntry* entry = self->entries[hash(key, self->capacity)];

    for (; entry != NULL; entry = entry->next) {
        if (strcmp(key, entry->key) == 0) {
            return entry;
        }
    }

    return NULL;
}

/**
 *
 */
Table* newTable(unsigned int capacity) {
    Table* table = pmalloc(sizeof (Table));

    *table = (Table) {
        .size = 0,
        .capacity = capacity,
        .insert = &insert,
        .lookup = &lookup,
        .entries = pmalloc(sizeof (TableEntry*) * capacity)
    };

    return table;
}

