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
    l->length = 0;
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

    if (index < 0 || index > l->length) {
        return;
    }

    node *new = malloc(sizeof(node));
    new->data = malloc(sizeof(l->element_size));
    new->next = NULL;
    memcpy(new->data, data, l->element_size);

    if (index == 0) {
        new->next = l->head;
        l->head = new;
    } else if (index == l->length) {
        l->tail->next = new;
        l->tail = new;
    } else {
        int i;
        node *current;
        for (i = 0, current = l->head; i < index - 1; i++, current = current->next);
        new->next = current->next;
        current->next = new;
    }
    l->length++;
}

void
list_remove(list *l, int index) {

    if (index < 0 || index >= l->length) {
        return;
    }

    node *to_free;
    if (index == 0) {
        to_free = l->head;
        l->head = to_free->next;
    } else if (index < l->length - 1){
        int i;
        node *current;
        for (i = 0, current = l->head; i < index - 1; i++, current = current->next);
        to_free = current->next;
        current->next = current->next->next;
    } else {
        int i;
        node *current;
        for (i = 0, current = l->head; i < index - 1; i++, current = current->next);
        to_free = current->next;
        current->next = NULL;
    }
    if (l->free_node) {
        l->free_node(to_free->data);
    }
    free(to_free->data);
    free(to_free);
    l->length--;
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
