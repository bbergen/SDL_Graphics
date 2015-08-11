//
// Created by bryan on 7/27/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "util.h"

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
    return true;
}

static int8_t
print_test_node_params(void *data, void *param) {
    test_data *test = (test_data*) data;
    char *param_string = (char*) param;
    printf("Printing: %s\n", test->display_text);
    printf("Parameter: %s\n\n", param_string);
    return true;
}

static void
allocate_test_data(test_data *test, char *display_text, int x, int y) {

    test->display_text = malloc(strlen(display_text) + 1);
    test->x = malloc(sizeof(int));
    test->y = malloc(sizeof(int));

    strcpy(test->display_text, display_text);
    *test->x = x;
    *test->y = y;
}

static void
run_list_test(void) {
    printf("Starting List Tests...\n");

    list test_list;

    init_list(&test_list, sizeof(test_data), test_free);


    test_data first;
    test_data second;
    test_data third;

    allocate_test_data(&first, "First Data Struct", 1, 2);
    allocate_test_data(&second, "Second Data Struct", 2, 3);
    allocate_test_data(&third, "Third Data Struct", 4, 5);

    add(&test_list, &first);
    add(&test_list, &second);
    add(&test_list, &third);

    printf("Size of test_list: %d\n", list_size(&test_list));
    list_for_each(&test_list, print_test_node);
    list_for_each_with_param(&test_list, print_test_node_params, "Dynamic Parameter");

    printf("Address of allocated test_list head: %p\n", test_list.head);
    free_list(&test_list);
    printf("Address of freed test_list head: %p\n", test_list.head);

    printf("List Test Completed\n\n");
}

static void
run_itoa_test(void) {
    printf("Starting Util Tests...\n");

    int n = 123;
    char *expected = "0000123";
    char test[100];
    itoa(n, test, 7);
    printf("Expected: %s, Result: %s\n\n", expected, test);

    n = -54321;
    char test2[100];
    itoa(n, test2, 0);
    printf("Expected: %d, Result: %s\n\n", n, test2);

    printf("Util Tests Completed\n\n");
}

int
main(int argc, char **argv) {
    run_list_test();
    run_itoa_test();
    return EXIT_SUCCESS;
}