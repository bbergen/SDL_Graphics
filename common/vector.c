//
// Created by bryan on 9/2/15.
//

#include <stdlib.h>
#include <stdio.h>
#include "vector.h"
#include "common.h"

global const int DEFAULT_CAPACITY = 16;
global const int LOAD_FACTOR = 2;

struct _vector {
    void **data;
    int size;
    int capacity;
};

typedef struct _vector* _vector;

vector
vector_init(int capacity) {
    _vector this = malloc(sizeof(_vector));
    this->capacity = capacity + DEFAULT_CAPACITY;
    this->size = 0;
    this->data = malloc(sizeof(void*) * this->capacity);
    return this;
}

void
vector_resize(vector v, int capacity) {
    _vector this = v;

    if (this->capacity >= capacity) {
        return;
    }

    void **data = realloc(this->data, sizeof(void*) * capacity);
    if (data) {
        this->data = data;
        this->capacity = capacity;
    }
}

void
vector_add(vector v, void *element) {
    _vector this = v;
    if (this->capacity == this->size) {
        vector_resize(this, this->capacity * LOAD_FACTOR);
    }
    this->data[this->size++] = element;
}

void
vector_set(vector v, int index, void *element) {
    _vector this = v;
    if (index >= 0 && index < this->size) {
        this->data[index] = element;
    }
}

void*
vector_get(vector v, int index) {
    _vector this = v;
    if (index >= 0 && index < this->size) {
        return this->data[index];
    }
    return NULL;
}

void
vector_remove(vector v, int index) {
    _vector this = v;
    if (index < 0 || index >= this->size) {
        return;
    }

    this->data[index] = NULL;

    int i;
    for (i = 0; i < this->size; i++) {
        this->data[i] = this->data[i + 1];
        this->data[i + 1] = NULL;
    }

    this->size--;

    if (this->size > 0 && this->size == this->capacity >> 2) {
        vector_resize(this, this->capacity >> 1);
    }
}

void
vector_free(vector v) {
    if (v) {
        _vector this = v;
        free(this->data);
        free(this);
    }
}

int
vector_size(vector v) {
    return v ? ((_vector)v)->size : 0;
}

int8_t
vector_empty(vector v) {
    if (v) {
        return ((_vector)v)->size == 0;
    }
    return true;
}
