//
// Created by bryan on 9/6/15.
//

#include <map.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "preference.h"
#include "common.h"

#define FILE_NAME_LENGTH 100
#define PREF_DELIM "="

#ifdef _WIN32
#define DELIM_C '\\'
#else
#define DELIM_C '/'
#endif

#ifdef _WIN32
#define DELIM_S "\\"
#else
#define DELIM_S "/"
#endif

struct _pref {
    char* file;
    hash_map pref_map;
};

typedef struct _pref* pref;

void free_pref(preference p) {
    pref this = p;
    free_map(this->pref_map);
    free(this->file);
    free(this);
}

preference read_pref(char* file_name) {

    // initialize preference
    pref this = calloc(1, sizeof(struct _pref));
    this->file = calloc(FILE_NAME_LENGTH, sizeof(char));
    this->pref_map = init_map();

    // see if we can find the passed file name
    char *home = getenv("HOME");
    strcat(this->file, home);
    if (file_name[0] != DELIM_C) {
        strcat(this->file, DELIM_S);
    }
    strcat(this->file, file_name);
    FILE *pref_file = fopen(this->file, "r");
    printf("File to open: %s\n", this->file);
    if (pref_file) {
        char line[FILE_NAME_LENGTH];
        while(fscanf(pref_file, "%s", line) != EOF) {
            char *key = strtok(line, PREF_DELIM);
            char *value = strtok(NULL, PREF_DELIM);
            put(this->pref_map, key, value, strlen(value) + 1);
        }
        fclose(pref_file);
    }

    return this;
}

internal void
manage_directory(char *path, __mode_t permissions) {
    struct stat st = {};
    if (stat(path, &st) == -1) {
#ifdef DEBUG
        printf("Creating New Directory at: %s\n", path);
#endif
        mkdir(path, permissions);
    }
}

void write_pref(preference p) {

    // lets see if the file already exists
    pref this = p;
    char *file_name = this->file;
    char path[FILE_NAME_LENGTH] = {};

    if (strchr(file_name, DELIM_C)) {
        char *tmp = strtok(file_name, DELIM_S);
        char dir[FILE_NAME_LENGTH];
        strcpy(dir, tmp);
        char *file = strtok(NULL, DELIM_S);
        char *next = strtok(NULL, DELIM_S);
        strcat(path, DELIM_S);
        strcat(path, dir);
        do {
            // lets create any directories we need
            manage_directory(path, 0700);
            strcpy(dir, file);
            strcpy(file, next);
            strcat(path, DELIM_S);
            strcat(path, dir);
            next = strtok(NULL, DELIM_S);
        } while (next);
        manage_directory(path, 0700);
        strcat(path, DELIM_S);
        strcat(path, file);
    } else {
        strcat(path, file_name);
    }

    // finally we can create or open the file
    FILE *pref_file = fopen(path, "w");
    if (pref_file) {
        vector keys = key_set(this->pref_map);
        int i;
        for (i = 0; i < vector_size(keys); i++) {
            char *key = vector_get(keys, i);
            char *value = get(this->pref_map, key);
            fprintf(pref_file, "%s=%s\n", key, value);
        }
        vector_free(keys);
        fclose(pref_file);
    } else {
        //TODO convert to logging in issue #13
        fprintf(stderr, "Cannot Open File: %s\n", path);
    }
}

char* get_spref(preference p, char *key) {
    pref this = p;
    return get(this->pref_map, key);
}

int8_t
pref_empty(preference p) {
    pref this = p;
    return map_empty(this->pref_map);
}

void put_spref(preference p, char *key, char *value) {
    pref this = p;
    put(this->pref_map, key, value, strlen(value) + 1);
}
