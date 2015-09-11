//
// Created by bryan on 7/27/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <colors.h>
#include <list.h>
#include <util.h>
#include <map.h>
#include <vector.h>
#include <preference.h>
#include <random.h>

#define LOG_RED(s) printf("%s %s %s", ANSI_COLOR_RED, (s), ANSI_COLOR_RED)
#define LOG_GREEN(s) printf("%s%s%s", ANSI_COLOR_GREEN, (s), ANSI_COLOR_RESET)
#define LOG_YELLOW(s) printf("%s%s%s", ANSI_COLOR_YELLOW, (s), ANSI_COLOR_RESET)

typedef struct test_data {
    char *display_text;
    int x;
    int y;
} test_data;

internal void
test_free(void *data) {
    test_data *test = (test_data*) data;
    printf("Freeing: %s\n", test->display_text);
    free(test->display_text);
}

internal int8_t
print_test_data(void *data) {
    test_data *test = (test_data*) data;
    printf("Printing: %s\n", test->display_text);
    printf("X: %d, Y: %d\n\n", test->x, test->y);
    return true;
}

internal int8_t
print_test_node_params(void *data, void *param) {
    test_data *test = (test_data*) data;
    char *param_string = (char*) param;
    printf("Printing: %s\n", test->display_text);
    printf("Parameter: %s\n\n", param_string);
    return true;
}

internal void
allocate_test_data(test_data *test, char *display_text, int x, int y) {
    test->display_text = malloc(strlen(display_text) + 1);
    strcpy(test->display_text, display_text);
    test->x = x;
    test->y = y;
}

internal void
run_list_test(void) {
    LOG_YELLOW("Starting List Tests...\n");

    list test_list;

    list_init(&test_list, sizeof(test_data), test_free);


    test_data first;
    test_data second;
    test_data third;

    allocate_test_data(&first, "First Data Struct", 1, 2);
    allocate_test_data(&second, "Second Data Struct", 2, 3);
    allocate_test_data(&third, "Third Data Struct", 4, 5);

    list_add(&test_list, &first);
    list_add(&test_list, &second);
    list_add(&test_list, &third);

    printf("Size of test_list: %d\n", list_size(&test_list));
    list_for_each(&test_list, print_test_data);
    list_for_each_with_param(&test_list, print_test_node_params, "Dynamic Parameter");

    printf("Testing Add At\n");
    test_data head;
    test_data tail;
    test_data mid;

    allocate_test_data(&head, "Inserted at HEAD", 42, 42);
    allocate_test_data(&tail, "Inserted at TAIL", 99, 99);
    allocate_test_data(&mid, "Inserted at MID", 33, 33);

    printf("List Size: %d\n", test_list.length);
    list_add_at(&test_list, 0, &head);
    printf("List Size: %d\n", test_list.length);
    list_add_at(&test_list, list_size(&test_list), &tail);
    printf("List Size: %d\n", test_list.length);
    list_add_at(&test_list, 2, &mid);
    printf("List Size: %d\n", test_list.length);
    list_for_each(&test_list, print_test_data);
    printf("Add At Test Success!\n");

    printf("Testing Remove At\n");

    printf("List Size: %d\n", test_list.length);
    list_remove(&test_list, 0); // test head removal
    printf("List Size: %d\n", test_list.length);
    list_remove(&test_list, test_list.length - 1); // test tail removal
    printf("List Size: %d\n", test_list.length);
    list_remove(&test_list, 1); // test mid removal
    printf("List Size: %d\n", test_list.length);
    list_for_each(&test_list, print_test_data);

    printf("Remove At Test Success\n");

    printf("Address of allocated test_list head: %p\n", test_list.head);
    list_free(&test_list);
    printf("Address of freed test_list head: %p\n", test_list.head);

    LOG_GREEN("List Test Completed\n\n");
}

internal void
run_itoa_test(void) {
    LOG_YELLOW("Starting Util Tests...\n");

    int n = 123;
    char *expected = "0000123";
    char test[100];
    itoa(n, test, 7);
    printf("Expected: %s, Result: %s\n\n", expected, test);

    n = -54321;
    char test2[100];
    itoa(n, test2, 0);
    printf("Expected: %d, Result: %s\n\n", n, test2);

    LOG_GREEN("Util Tests Completed\n\n");
}

internal void
run_map_test(void) {
    LOG_YELLOW("Starting Map Tests...\n");

    hash_map map;
    map = map_init();

    // test put
    printf("Starting put test...\n");
    char *key_1 = "key_1";
    char *value_1 = "hello world!";
    map_put(map, key_1, value_1, strlen(value_1) + 1);
    printf("Expected: %s, Result: %s\n", value_1, (char*) map_get(map, key_1 ));
    printf("End put test\n\n");

    // test overwrite
    printf("Starting overwrite test...\n");
    char *value_2 = "good bye!";
    map_put(map, key_1, value_2, strlen(value_2) + 1);
    printf("Expected: %s, Result: %s\n", value_2, (char*) map_get(map, key_1));
    printf("End overwrite test\n\n");

    // test delete
    printf("Starting removal test...\n");
    char *key_2 = "KEY_TWO";
    map_put(map, key_2, value_2, strlen(value_2) + 1);
    map_remove(map, key_2);
    printf("Entry Removal %s\n", map_get(map, key_2) ? "Failed" : "Succeeded");

    // test idempotency of delete
    map_remove(map, key_2);
    printf("End removal test\n\n");

    // test map growth
    printf("Starting growth test...\n");
    char buffer[512];
    int i;
    for (i = 0; i < 10000; i++) {
        sprintf(buffer, "%d", i);
        map_put(map, buffer, buffer, strlen(buffer));
    }
    for (i = 0; i < 10000; i += 1000) {
        sprintf(buffer, "%d", i);
        printf("Stored Value: %s\n", (char*) map_get(map, buffer));
    }
    printf("End growth test\n\n");

    //test map freeing
    map_free(map);

    // new map
    map =  map_init();

    // testing struct values
    test_data data_1;
    test_data data_2;
    test_data data_3;
    allocate_test_data(&data_1, "Test Struct One!", 42, 42);
    allocate_test_data(&data_2, "Test Struct Two!", 11, 21);
    allocate_test_data(&data_3, "Test Struct Three!", 400, -67);
    char *key_3 = "struct_key";
    char *key_4 = "duplicate_key";
    char *key_5 = "duplicate_key";

    printf("Starting struct value test...\n");
    map_put(map, key_3, &data_1, sizeof(test_data));
    map_put(map, key_4, &data_2, sizeof(test_data));
    map_put(map, key_5, &data_3, sizeof(test_data));

    printf("Expected: \n");
    print_test_data(&data_1);
    printf("Result: \n");
    print_test_data(map_get(map, key_3));

    printf("Expected: \n");
    print_test_data(&data_3);
    printf("Result: \n");
    print_test_data(map_get(map, key_4));

    printf("Expected: \n");
    print_test_data(&data_3);
    printf("Result: \n");
    print_test_data(map_get(map, key_5));

    test_free(&data_1);
    test_free(&data_2);
    test_free(&data_3);
    printf("End struct value test\n\n");
    map_free(map);

    LOG_GREEN("Map Tests Completed\n\n");
}

internal void
run_vector_test(void) {
    LOG_YELLOW("Starting Vector Tests...\n");

    printf("Starting Vector Initialization Tests\n");
    vector v = vector_init(10);
    vector_add(v, "Test");
    vector_add(v, "Vector");
    vector_add(v, "Please");
    vector_add(v, "Ignore");
    printf("Vector Initialization Successful\n");

    printf("Starting Standard Iteration Tests\n");
    int i;
    for (i = 0; i < vector_size(v); i++) {
        printf("%s ", (char*) vector_get(v, i));
    }
    printf("\n");

    test_data data1;
    test_data data2;
    test_data data3;
    test_data data4;
    vector v2 = vector_init(0);
    allocate_test_data(&data1, "Test", 1, 2);
    vector_add(v2, &data1);
    allocate_test_data(&data2, "Vector", 3, 4);
    vector_add(v2, &data2);
    allocate_test_data(&data3, "Please", 5, 6);
    vector_add(v2, &data3);
    allocate_test_data(&data4, "Ignore", 7, 8);
    vector_add(v2, &data4);

    for (i = 0; i < vector_size(v2); i++) {
        print_test_data(vector_get(v2, i));
    }

    printf("\nStandard Iteration Success!\n");

    printf("Starting Vector Set & Remove Tests\n");

    vector_remove(v, 3);
    vector_remove(v, 2);
    vector_remove(v, 1);
    vector_set(v, 0, "Hello");
    vector_add(v, "World!");

    for (i = 0; i < vector_size(v); i++) {
        printf("%s ", (char*) vector_get(v, i));
    }
    printf("\nVector Set & Remove Tests Success!\n");

    printf("Starting Vector Resize Tests\n");
    vector v3 = vector_init(0);
    for (i = 0; i < 10000; i++) {
        vector_add(v3, "TEST_STRING");
    }
    printf("Vector Size Expected: %d, Actual: %d\n", 10000, vector_size(v3));
    printf("Vector Resize Test Success!\n");

    printf("Starting Vector Free Test\n");
    vector_free(v);
    vector_free(v2);
    vector_free(v3);
    printf("Vector Free Test Success!\n");
    LOG_GREEN("Vector Tests Completed\n\n");
}

internal void
run_pref_test(void) {

    LOG_YELLOW("Starting Preference Test\n");
    char *test_file = "/.sdl/unit/test/preference.config";

    printf("Starting Preference Read Test\n");
    preference prefs = read_pref(test_file);
    printf("Preferences Read\n\n");

    if (!pref_empty(prefs)) {
        printf("Starting Preference Get Tests\n");
        printf("Expected: %s, result: %s\n", "preferences", get_spref(prefs, "testing"));
        printf("Expected: %s, result: %s\n", "pref_test", get_spref(prefs, "another"));
        printf("Expected: %s, result: %s\n", "test!", get_spref(prefs, "third"));
        printf("Preferences Retrieved\n\n");
    }

    printf("Starting Preference Put Test\n");
    put_spref(prefs, "testing", "preferences");
    put_spref(prefs, "another", "pref_test");
    put_spref(prefs, "third", "test!");
    printf("Preferences Put\n\n");

    printf("Starting Preference Write Test\n");
    write_pref(prefs);
    printf("Preferences Written\n\n");

    printf("Starting Preferences Free Test\n");
    free_pref(prefs);
    printf("Preferences Freed\n");
    LOG_GREEN("Preference Tests Completed\n\n");
}

internal void
run_random_test(void) {
    LOG_YELLOW("Starting Random Test\n");
    int i;
    for (i = 0; i < 100; i++) {
        printf("Min: %d, Max: %d, Value: %d\n", 10, 30, (int) random_in_range(10, 30));
        printf("Random Double: %f\n", random_double());
    }
    LOG_GREEN("Random Test Completed\n");
}

int
main(int argc, char **argv) {
    run_list_test();
    run_itoa_test();
    run_map_test();
    run_vector_test();
    run_pref_test();
    run_random_test();
    return EXIT_SUCCESS;
}