//
// Created by bryan on 9/2/15.
//

#ifndef SDL_GRAPHICS_VECTOR_H
#define SDL_GRAPHICS_VECTOR_H

/*
 *  Hidden Implementation
 */
typedef void* vector;

/*
 * Initializes a new vector
 *
 * arg 1:   Initial capacity of the vector, above and beyond the default capacity
 * arg 2:   Size of an individual element contained in the vector
 * return:  An initialized vector
 */
vector init_vector(int, size_t);

/*
 * Increases the size of the vector
 *
 * arg 1:   The vector to resize
 * arg 2:   The quantity to increase the size by
 */
void resize_vector(vector, int);

/*
 * Increases the capacity of the vector
 * In most cases it is not necessary to call this manually.
 * It will be called automatically when the size exceeds the capacity
 *
 * arg 1:   The vector to expand
 * arg 2:   The quantity to increase the capacity by
 */
void reserve_vector(vector, int);

/*
 * Adds a new element at the end of the vector
 *
 * arg 1:   The vector to add to
 * arg 2:   The element to add
 */
void add(vector, void*);

/*
 * Adds all of an array to the end of the vector
 *
 * arg 1:   The vector to add to
 * arg 2:   The array of elements to add
 * arg 3:   Length of the array added
 */
void add_all(vector, void*, int);

/*
 * Fetches an element from the vector
 *
 * arg 1:   The vector to fetch from
 * arg 2:   The index of the element to fetch
 * return:  The element
 */
void* get(vector, int);

/*
 * Returns the size of the vector
 *
 * arg 1:   The vector to measure
 * return:  The number of elements in the vector
 */
int size(vector);

#endif //SDL_GRAPHICS_VECTOR_H
