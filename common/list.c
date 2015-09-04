//
// Created by bryan on 7/27/15.
//

#include <stdlib.h>
#include <string.h>
#include "list.h"

void
list_init(list *l, size_t element_size, free_function free_node) {
    assert(element_size > 0);
    l->length = 0;
    l->element_size = element_size;
    l->head = NULL;
    l->tail = NULL;
    l->free_node = free_node;
}

int
list_size(list *l) {
    return l->length;
}

void
list_free(list *l) {
    node *current;
    while (l->head != NULL) {
        current = l->head;
        l->head = current->next;

        if (l->free_node) {
            l->free_node(current->data);
        }

        free(current->data);
        free(current);
    }
}

void
list_add(list *l, void *data) {
    node *new = malloc(sizeof(node));
    new->data = malloc(l->element_size);
    new->next = NULL;

    memcpy(new->data, data, l->element_size);

    if (l->length == 0) {
        l->head = new;
        l->tail = new;
    } else {
        l->tail->next = new;
        l->tail = new;
    }

    l->length++;
}

void
list_add_at(list *l, int index, void *data) {
    //TODO implement
}

void
list_remove(list *l, int index) {
    //TODO implement
}

void
list_for_each(list *l, list_iterator iterator) {
    assert(iterator != NULL)

    node *current = l->head;
    int8_t result = true;
    while (current && result) {
        result = iterator(current->data);
        current = current->next;
    }
}

void
list_for_each_with_param(list *l, list_iterator_with_param iterator, void *param) {
    assert(iterator != NULL)

    node *current = l->head;
    int8_t result = true;
    while (current && result) {
        result = iterator(current->data, param);
        current = current->next;
    }
}
