//
// Created by bryan on 7/27/15.
//

#ifndef SDL_GRAPHICS_LIST_H
#define SDL_GRAPHICS_LIST_H

#include <stdint.h>
#include <stddef.h>
#include "common.h"

typedef void (*free_function)(void *);

typedef int8_t (*list_iterator)(void *);

typedef int8_t (*list_iterator_with_param)(void *, void *);

typedef struct _node {
    void *data;
    struct _node *next;
} node;

typedef struct {
    int length;
    size_t element_size;
    node *head;
    node *tail;
    free_function free_node;
} list;

/*
 * Initializes a list struct,
 * Passed function should free an individual node's memory
 */
void list_init(list*, size_t, free_function);

/*
 * Iterates over the list and calls its free_function
 * on each element.
 */
void list_free(list*);

/*
 * Iterates over the list and calls the passed function
 * on each element
 */
void list_for_each(list*, list_iterator);

/*
 * Iterates over the list and calls the passed function
 * on each element, additionally passing it the third argument
 */
void list_for_each_with_param(list*, list_iterator_with_param, void *);

/*
 * Appends an element to the list
 */
void list_add(list*, void*);

/*
 * Inserts an element at a specific index
 */
void list_add_at(list*, int, void*);

/*
 * Removes the element at the specific index
 */
void list_remove(list*, int);

/*
 * Returns the number of elements in the list
 */
int list_size(list *l);

#endif //SDL_GRAPHICS_LIST_H
