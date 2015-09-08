//
// Created by bryan on 8/12/15.
//

#include <stdint-gcc.h>
#include <stddef.h>
#include "vector.h"

#ifndef SDL_GRAPHICS_MAP_H
#define SDL_GRAPHICS_MAP_H

/*
 *  Map structure, simple implementation that holds
 *  any type of value accessed via a string key
 */
typedef struct hash_map *hash_map;

/*
 * Initializes a new hash map.
 */
hash_map map_init(void);

/*
 * Frees all memory associated with the hash map
 */
void map_free(hash_map);

/*
 * Inserts a key value pair into the map.
 * Duplicate keys will have their values overwritten
 * Keys can be any value excluding NULL
 */
void map_put(hash_map, const char *, void *, size_t);

/**
 * Returns the value associated with the string key
 * Returns NULL of the key matches no value
 */
void* map_get(hash_map, const char *);

/*
 * Removes the key value pair associated with passed key
 * Currently only removes a single value from a bucket.
 * If there was a collision, only the most recent entry will
 * be removed, and multiple calls are needed to remove chained values
 */
void map_remove(hash_map, const char *);

/*
 * Returns true (1) if the map contains the passed key, false (0) otherwise
 * A true result from this call does not guarantee that the value is not NULL
 */
int8_t map_contains(hash_map, const char *);

/*
 * Return the full set of keys contained by the hashmap
 * It is the client's responsibility to free the returned vector
 */
vector key_set(hash_map);

/*
 * Returns whether the map is in an empty state
 */
int8_t map_empty(hash_map);

#endif //SDL_GRAPHICS_MAP_H
