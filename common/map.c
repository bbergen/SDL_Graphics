//
// Created by bryan on 8/12/15.
//

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "map.h"

typedef struct entry {
    struct entry *next;
    char *key;
    void *value;
} entry;

struct hash_map {
    int size;
    int n;
    entry **hash_table;
};

#define INITIAL_SIZE 1024
#define GROWTH_RATE 2
#define MAX_LOAD_FACTOR 1

internal hash_map
_init_map(int size) {
    //TODO implement
}

hash_map
init_map(void) {
    return _init_map(INITIAL_SIZE);
}

void
free_map(hash_map) {
    //TODO implement
}

#define PRIME 97

internal uint64_t
hash_function(const char *key) {
    const u_char *i;
    uint64_t hash = 0;

    for (i = (const u_char *) key; *i; i++) {
        hash = hash * PRIME + *i;
    }

    return hash;
}

internal void
resize_map(hash_map) {
    //TODO implement
}

void
put(hash_map map, const char *key, void *value) {
    //TODO implement
}

void*
get(hash_map map, const char *key) {
    //TODO implement
}

void
remove(hash_map, const char *key) {
    //TODO implement
}

int8_t
contains(hash_map map, const char *key) {
    //TODO implement
}
