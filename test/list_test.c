//
// Created by bryan on 7/27/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct test_data {
    char *display_text;
    int *x;
    int *y;
} test_data;

static void
test_free(void *data) {
    test_data *test = (test_data*) data;
    printf("Freeing: %s\n", test->display_text);

    free(test->display_text);
    free(test->x);
    free(test->y);
}

static int8_t
print_test_node(void *data) {
    test_data *test = (test_data*) data;
    printf("Printing: %s\n", test->display_text);
    printf("X: %d, Y: %d\n\n", *test->x, *test->y);
}

static test_data*
allocate_test_data(char *display_text, int x, int y) {
    test_data *test = malloc(sizeof(test_data));

    test->display_text = malloc(strlen(display_text) + 1);
    test->x = malloc(sizeof(int));
    test->y = malloc(sizeof(int));

    strcpy(test->display_text, display_text);
    *test->x = x;
    *test->y = y;

    return test;
}

int
main(int argc, char **argv) {
    printf("Starting List Tests...\n");

    list test_list;

    init_list(&test_list, sizeof(test_data), test_free);

    test_data *first = allocate_test_data("First Data Struct", 1, 2);
    test_data *second = allocate_test_data("Second Data Struct", 2, 3);
    test_data *third = allocate_test_data("Third Data Struct", 4, 5);

    add(&test_list, first);
    add(&test_list, second);
    add(&test_list, third);

    printf("Size of test_list: %d\n", list_size(&test_list));
    list_for_each(&test_list, print_test_node);

    printf("Address of allocated test_list head: %p\n", test_list.head);
    free_list(&test_list);
    printf("Address of freed test_list head: %p\n", test_list.head);

    printf("List Test Completed\n");
    return EXIT_SUCCESS;
}