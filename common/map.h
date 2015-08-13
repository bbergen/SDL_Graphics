//
// Created by bryan on 8/12/15.
//

#include <stdint-gcc.h>

#ifndef SDL_GRAPHICS_MAP_H
#define SDL_GRAPHICS_MAP_H

typedef struct hash_map hash_map;

hash_map init_map(void);

void free_map(hash_map);

void put(hash_map, char *, void *);

void* get(hash_map, const char *);

void remove(hash_map, const char *);

int8_t contains(hash_map, const char *);

#endif //SDL_GRAPHICS_MAP_H
