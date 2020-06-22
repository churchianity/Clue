
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clue.h"
#include "table.h"
#include "util.h"

/**
 * Implementation stolen from:
 * https://stackoverflow.com/a/107657
 *
 * Though the same implementation (except replace 101 with 31) exists on page 118 of K&R,
 * and the hash table implementation here is ripped from that section of the book.
 */
static unsigned int hash(const char* value, unsigned int capacity /*, unsigned int seed = 0*/ ) {
    unsigned int hash = 0; // seed

    while (*value) {
        hash = hash * 101 + *value++;
    }

    return hash % capacity;
}

/**
 *
 */
static signed int insert(Table* table, const char* value, unsigned int capacity) {
    TableEntry* entry = lookup(table, value, capacity);

    if (!entry) {
        entry = pmalloc(sizeof (TableEntry));
        entry->value = value;

        unsigned int hashValue = hash(value, capacity);
        table[hashValue] = entry;

    } else {
        return -1;
    }

    return 0;
}

/**
 *
 */
static TableEntry* lookup(Table* table, const char* value, unsigned int capacity) {
    for (TableEntry* p = table[hash(value, capacity)]; p != NULL; p = p->next) {
        if (strcmp(value, p->value) == 0) {
            return p;
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
        .entries = pmalloc(sizeof (TableEntry) * capacity);
        .capacity = capacity
    };

    return table;
}

