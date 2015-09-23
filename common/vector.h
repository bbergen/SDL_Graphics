//
// Created by bryan on 9/2/15.
//

#ifndef SDL_GRAPHICS_VECTOR_H
#define SDL_GRAPHICS_VECTOR_H

#include <stdint.h>

/*
 *  Hidden Implementation
 */
typedef void* vector;

/*
 *  Tool for iterating through vector
 */
typedef void* iterator;

/*
 * Initializes a new vector
 *
 * arg 1:   Initial capacity of the vector, above and beyond the default capacity
 * return:  An initialized vector
 */
vector vector_init(int);

/*
 * Frees the memory associated with the vector
 */
void vector_free(vector);

/*
 * Increases the size of the vector
 *
 * arg 1:   The vector to resize
 * arg 2:   The quantity to increase the size by
 */
void vector_resize(vector, int);

/*
 * Adds a new element at the end of the vector
 *
 * arg 1:   The vector to add to
 * arg 2:   The element to add
 */
void vector_add(vector, void*);

/*
 * Swaps an element at the specified index
 * Note, that this function will not free the
 * memory of the original index
 *
 * arg 1:   The vector to add to
 * arg 2:   The index at which to swap
 * arg 3:   The element to add
 */
void vector_set(vector, int, void*);

/*
 * Removes the element at the index from the vector
 * Will not free the memory pointed to by the index
 *
 * arg 1:   The vector to delete from
 * arg 2:   The index of the element to remove
 */
void vector_remove(vector, int);

/*
 * Fetches an element from the vector
 *
 * arg 1:   The vector to fetch from
 * arg 2:   The index of the element to fetch
 * return:  The element
 */
void* vector_get(vector, int);

/*
 * Returns the size of the vector
 *
 * arg 1:   The vector to measure
 * return:  The number of elements in the vector
 */
int vector_size(vector);

/*
 * Returns true if the vector has no elements, false otherwise
 */
int8_t vector_empty(vector);

#endif //SDL_GRAPHICS_VECTOR_H
