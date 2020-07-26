
#ifndef TABLE_H
#define TABLE_H

#include "alloc.h"
#include "types.h"


template <class K, class V>
struct TableEntry {
    TableEntry<K, V>* next;
    K* key;
    V* value;
};

template <class K, class V>
struct Table {
    u32 lanes;
    TableEntry<K, V>** entries;

    /**
     * @NOTE
     */
    Table<K, V>(u32 _lanes) {
        lanes = _lanes;
        entries = (TableEntry<K, V>**) pCalloc(lanes, sizeof (TableEntry<K, V>*));
    }

    static u32 hash(K* key, u32 capacity) {
        u32 hash = 0;

        while (*key) {
            hash = hash * 31 + *key++;
        }

        return hash % capacity;
    }

    signed int insert(K* key, V* value) {
        TableEntry<K, V>* entry = lookup(key);

        u32 hashValue;

        if (!entry) {
            entry = (TableEntry<K, V>*) pMalloc(sizeof (TableEntry<K, V>));
            entry->key = key;
            entry->value = value;

            hashValue = hash(key, lanes);
            entry->next = entries[hashValue];
            entries[hashValue] = entry;

            return 0;

        } else {
            free(entry->value);

            entry->value = value;
            return 1;
        }
    }

    TableEntry<K, V>* lookup(K* key) {
        TableEntry<K, V>* entry = entries[hash(key, lanes)];

        for (; entry != null; entry = entry->next) {

            bool equal = true;

            for (u32 i = 0; i < sizeof (K); i++) {
                if (entry->key[i] != key[i]) {
                    equal = false;
                    break;
                }
            }

            if (equal) {
                return entry;
            }

            // if (streq(key, entry->key)) {
            //     return entry;
            // }
        }

        return null;
    }

    void clear() {
        for (u32 i = 0; i < lanes; i++) {
            TableEntry<K, V>* entry = entries[i];
            TableEntry<K, V>* prev;

            while (entry) {
                prev = entry;
                free(prev);
                entry = entry->next;
            }
        }

        entries = (TableEntry<K, V>**) pCalloc(lanes, sizeof (TableEntry<K, V>*));
    }
};

#endif

