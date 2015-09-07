//
// Created by bryan on 9/6/15.
//

#ifndef SDL_GRAPHICS_PREFERENCE_H
#define SDL_GRAPHICS_PREFERENCE_H

/*
 * Hide internal implementation of preference struct
 */
typedef void* preference;

/*
 * Initialize a preference struct from a file
 * If the file doesn't exist, a new preference will be
 * created
 */
preference read_pref(char*);

/*
 * Write the passed preference to the passed file
 */
void write_pref(preference);

/*
 * Retrieve a value from the preference map
 */
char* get_spref(preference, char*);

/*
 * Save a key value pair to a preference map
 */
void put_spref(preference, char*, char*);

/*
 * Free memory associated with the preference map
 */
void free_pref(preference);

/*
 * Asks if the preference map is in an empty state
 */
int8_t pref_empty(preference);

#endif //SDL_GRAPHICS_PREFERENCE_H
