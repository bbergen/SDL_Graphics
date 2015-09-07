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
    size_t value_size;
} entry;

struct hash_map {
    int size;
    int n;
    entry **hash_table;
    vector key_set;
};

#define INITIAL_SIZE 1024
#define GROWTH_RATE 2
#define MAX_LOAD_FACTOR 1

internal hash_map
_init_map(int size) {
    hash_map map;
    int i;

    map = malloc(sizeof(*map));

    if (map) {
        map->size = size;
        map->n = 0;
        map->key_set = vector_init(size);
        map->hash_table = malloc(sizeof(entry*) * map->size);

        if (map->hash_table) {
            for (i = 0; i < map->size; ++i) {
                map->hash_table[i] = 0;
            }
        }
    }
    return map;
}

hash_map
init_map(void) {
    return _init_map(INITIAL_SIZE);
}

void
free_map(hash_map map) {

    int i;
    entry *e;
    entry *next;

    for (i = 0; i < map->size; ++i) {
        for (e = map->hash_table[i]; e; e = next) {
            next = e->next;
            free(e->key);
            free(e->value);
            free(e);
        }
    }

    free(map->hash_table);
    vector_free(map->key_set);
    free(map);
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
resize_map(hash_map map) {
    hash_map new_map;
    struct hash_map tmp;
    int i;
    entry *e;

    new_map = _init_map(map->size * GROWTH_RATE);

    for (i = 0; i < map->size; ++i) {
        for (e = map->hash_table[i]; e; e = e->next) {
            // just a straight up copy for now... slow
            put(new_map, e->key, e->value, e->value_size);
        }
    }

    tmp = *map;
    *map = *new_map;
    *new_map = tmp;
    free_map(new_map);
}

void
put(hash_map map, const char *key, void *value, size_t value_size) {
    entry *e;
    uint64_t hash;
    if (key && value) {
        vector_add(map->key_set, key);
        e = malloc(sizeof(entry));
        if (e) {
            e->key = strdup(key);
            e->value = malloc(value_size);
            e->value_size = value_size;
            if (e->value) {
                memcpy(e->value, value, value_size);
                hash = hash_function(key) % map ->size;
                e->next = map->hash_table[hash];
                map->hash_table[hash] = e;
                map->n++;

                if (map->n >= map->size * MAX_LOAD_FACTOR) {
                    resize_map(map);
                }
            }
        }
    }
}

void*
get(hash_map map, const char *key) {
    entry *e;
    for (e = map->hash_table[hash_function(key) % map->size]; e; e = e->next) {
        if (!strcmp(e->key, key)) {
            // found!
            return e->value;
        }
    }
    return NULL;
}

void
remove_entry(hash_map map, const char *key) {
    //TODO make this delete all chained values if there were collisions.
    entry **prev;
    entry *e;

    for (prev = &(map->hash_table[hash_function(key) % map->size]); *prev; prev = &((*prev)->next)) {
        if (!strcmp((*prev)->key, key)) {
            // found!
            e = *prev;
            *prev = e->next;
            free(e->key);
            free(e->value);
            free(e);
            return;
        }
    }
}

int8_t
map_empty(hash_map map) {
    return map->n <= 0;
}

int8_t
contains(hash_map map, const char *key) {
    return get(map, key) != NULL;
}

vector
key_set(hash_map map) {
    vector new_vector = vector_init(vector_size(map->key_set));
    int i;
    for (i = 0; i < vector_size(map->key_set); i++) {
        if (contains(map, vector_get(map->key_set, i))) {
            vector_add(new_vector, vector_get(map->key_set, i));
        }
    }
    return new_vector;
}
