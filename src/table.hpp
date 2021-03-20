
#ifndef TABLE_H
#define TABLE_H

#include <new>

#include "alloc.h"
#include "string.h"
#include "types.h"


template <class K, class V>
struct TableEntry {
    TableEntry<K, V>* next;

    u32 keyLength;

    K* key;
    V* value;
};

template <class K, class V>
struct Table {
    u32 lanes;
    TableEntry<K, V>** entries;

    Table<K, V>(u32 _lanes = 10) { // the default number of 'lanes' of 10 is arbitrary
        lanes = _lanes;
        entries = (TableEntry<K, V>**) pCalloc(sizeof (TableEntry<K, V>*), lanes);
    }

    void* operator new(size_t size) {
        return (Table<K,V>*) pMalloc(sizeof (Table<K,V>));
    }

    void operator delete(void* p) {
        // @TODO free the entries first
        pFree(p);
    }

    static u32 hash(K* key, u32 keyLength, u32 capacity) {
        u32 hash = 0;

        for (u32 i = 0; i < keyLength; i++) {
            hash = hash * 31 + *key++;
        }

        return hash % capacity;
    }

    signed int insert(K* key, u32 keyLength, V* value) {
        TableEntry<K, V>* entry = lookup(key, keyLength);

        u32 hashValue;

        if (!entry) {
            entry = (TableEntry<K, V>*) pMalloc(sizeof (TableEntry<K, V>));
            entry->key = key;
            entry->keyLength = keyLength;
            entry->value = value;

            hashValue = hash(key, keyLength, lanes);
            entry->next = entries[hashValue];
            entries[hashValue] = entry;

            return 0;

        } else {
            pFree(entry->value);

            entry->value = value;
            return 1;
        }
    }

    /**
     * @FIXME
     * this is completely and totally unsafe for non char* keys, and probably other weird values too.
     * we want to support arbitrary data - we could do this (with minor gotchas) by passing the size
     * of the key and the size of the value at init time, then we can check for equality on some POD.
     *
     * we would need an array wrapper so we can know the length/sizeof array keys...
     * if we could store two keys that are bitwise identical, but represent different encodings, it breaks...
     *
     * for example this float ...000101010100101
     * versus the integer     ...000101010100101
     *
     * mean very different things to the user, but is the same piece of POD.
     */
    TableEntry<K, V>* lookup(K* key, u32 keyLength) {
        TableEntry<K, V>* entry = entries[hash(key, keyLength, lanes)];

        for (; entry != null; entry = entry->next) {
            if (Str :: memeq(key, keyLength, entry->key, entry->keyLength)) {
                return entry;
            }
        }

        return null;
    }

    void clear(void (*entryCleanupFunction)(TableEntry<K, V>*) = ([] (TableEntry<K, V>* e) {})) {
        for (u32 i = 0; i < lanes; i++) {
            TableEntry<K, V>* entry = entries[i];
            TableEntry<K, V>* prev;

            while (entry) {
                entryCleanupFunction(entry);

                prev = entry;
                pFree(prev);
                entry = entry->next;
            }
        }

        entries = (TableEntry<K, V>**) pCalloc(sizeof (TableEntry<K, V>*), lanes);
    }

    void traverse(void (*keyCallback) (K*), void (*valueCallback) (V*)) {
        for (u32 i = 0; i < lanes; i++) {
            TableEntry<K, V>* entry = entries[i];

            while (entry) {
                if (keyCallback) {
                    keyCallback(entry->key);
                }

                if (valueCallback) {
                    valueCallback(entry->value);
                }

                entry = entry->next;
            }
        }
    }
};

#endif

