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
#include <preference.h>
#include <random.h>
#include <unistd.h>

#define ACCEPTABLE_THRESHOLD 1.0

#define LOG_RED(s) printf("%s%s%s", ANSI_COLOR_RED, (s), ANSI_COLOR_RESET)
#define LOG_GREEN(s) printf("%s%s%s", ANSI_COLOR_GREEN, (s), ANSI_COLOR_RESET)
#define LOG_YELLOW(s) printf("%s%s%s", ANSI_COLOR_YELLOW, (s), ANSI_COLOR_RESET)

#define ASSERT_TRUE(val) if (!(val)) { return false; }
#define ASSERT_FALSE(val) if ((val)) { return false; }
#define ASSERT_I_EQUAL(x, y) if ((x) != (y)) { return false; }
#define ASSERT_S_EQUAL(x, y) if (strcmp((x), (y)) != 0) { return false; }

typedef int8_t (*test_function)(void);

typedef struct test_data {
    char *display_text;
    int x;
    int y;
} test_data;

internal int8_t
test_data_equals(test_data *first, test_data *second) {
    if (first->x != second->x) {
        return false;
    }

    if (first->y != second->y) {
        return false;
    }

    return strcmp(first->display_text, second->display_text) == 0;
}

internal void
test_free(void *data) {
    test_data *test = (test_data*) data;
    free(test->display_text);
}

internal void
allocate_test_data(test_data *test, char *display_text, int x, int y) {
    test->display_text = malloc(strlen(display_text) + 1);
    strcpy(test->display_text, display_text);
    test->x = x;
    test->y = y;
}

internal int8_t
for_each_list(void *node, void *param) {
    int *ptr = param;
    (*ptr)++;
    return true;
}

internal int8_t
run_list_test(void) {

    list test_list;
    list_init(&test_list, sizeof(test_data), test_free);
    ASSERT_TRUE(&test_list);

    test_data first;
    test_data second;
    test_data third;

    allocate_test_data(&first, "First Data Struct", 1, 2);
    allocate_test_data(&second, "Second Data Struct", 2, 3);
    allocate_test_data(&third, "Third Data Struct", 4, 5);

    list_add(&test_list, &first);
    list_add(&test_list, &second);
    list_add(&test_list, &third);
    ASSERT_I_EQUAL(list_size(&test_list), 3);

    int count = 0;
    list_for_each_with_param(&test_list, for_each_list, &count);
    ASSERT_I_EQUAL(count, list_size(&test_list));

    test_data head;
    test_data tail;
    test_data mid;

    allocate_test_data(&head, "Inserted at HEAD", 42, 42);
    allocate_test_data(&tail, "Inserted at TAIL", 99, 99);
    allocate_test_data(&mid, "Inserted at MID", 33, 33);

    list_free(&test_list);
    return true;
}

internal int8_t
run_itoa_test(void) {
    int n = 123;
    char *expected = "0000123";
    char test[100];
    itoa(n, test, 7);
    ASSERT_S_EQUAL(expected, test);

    n = -54321;
    char test2[100];
    itoa(n, test2, 0);
    char ex[10] = {};
    sprintf(ex, "%d", n);
    ASSERT_S_EQUAL(ex, test2);

    return true;
}

internal int8_t
run_map_test(void) {

    hash_map map = map_init();
    ASSERT_TRUE(map);

    char *key_1 = "key_1";
    char *value_1 = "hello world!";
    map_put(map, key_1, value_1, strlen(value_1) + 1);
    ASSERT_S_EQUAL(value_1, map_get(map, key_1));

    char *value_2 = "good bye!";
    map_put(map, key_1, value_2, strlen(value_2) + 1);
    ASSERT_S_EQUAL(value_2, map_get(map, key_1));

    char *key_2 = "KEY_TWO";
    map_put(map, key_2, value_2, strlen(value_2) + 1);
    map_remove(map, key_2);
    ASSERT_FALSE(map_get(map, key_2));
    map_remove(map, key_2);
    ASSERT_FALSE(map_get(map, key_2));

    char buffer[512] = {};
    int i;
    for (i = 0; i < 10000; i++) {
        sprintf(buffer, "%d", i);
        map_put(map, buffer, buffer, strlen(buffer));
        ASSERT_TRUE(map_get(map, buffer));
    }

    map_free(map);
    map =  map_init();
    ASSERT_TRUE(map);

    test_data data_1;
    test_data data_2;
    test_data data_3;
    allocate_test_data(&data_1, "Test Struct One!", 42, 42);
    allocate_test_data(&data_2, "Test Struct Two!", 11, 21);
    allocate_test_data(&data_3, "Test Struct Three!", 400, -67);
    char *key_3 = "struct_key";
    char *key_4 = "duplicate_key";
    char *key_5 = "duplicate_key";

    map_put(map, key_3, &data_1, sizeof(test_data));
    map_put(map, key_4, &data_2, sizeof(test_data));
    map_put(map, key_5, &data_3, sizeof(test_data));

    test_data data[] = {
            {"Test Struct One!", 42, 42},
            {"Test Struct Two!", 11, 21},
            {"Test Struct Three!", 400, -67}
    };

    ASSERT_TRUE(test_data_equals(&data[0], map_get(map, key_3)));
    ASSERT_TRUE(test_data_equals(&data[2], map_get(map, key_4)));
    ASSERT_TRUE(test_data_equals(&data[2], map_get(map, key_5)));

    test_free(&data_1);
    test_free(&data_2);
    test_free(&data_3);
    map_free(map);

    return true;
}

internal int8_t
run_vector_test(void) {

    vector v = vector_init(10);

    ASSERT_TRUE(v);
    vector_add(v, "Test");
    vector_add(v, "Vector");
    vector_add(v, "Please");
    vector_add(v, "Ignore");
    ASSERT_I_EQUAL(vector_size(v), 4);

    ASSERT_S_EQUAL("Test", vector_get(v, 0));
    ASSERT_S_EQUAL("Vector", vector_get(v, 1));
    ASSERT_S_EQUAL("Please", vector_get(v, 2));
    ASSERT_S_EQUAL("Ignore", vector_get(v, 3));

    test_data data1;
    test_data data2;
    test_data data3;
    test_data data4;
    vector v2 = vector_init(0);
    ASSERT_TRUE(v2);
    allocate_test_data(&data1, "Test", 1, 2);
    vector_add(v2, &data1);
    allocate_test_data(&data2, "Vector", 3, 4);
    vector_add(v2, &data2);
    allocate_test_data(&data3, "Please", 5, 6);
    vector_add(v2, &data3);
    allocate_test_data(&data4, "Ignore", 7, 8);
    vector_add(v2, &data4);
    ASSERT_I_EQUAL(vector_size(v2), 4);

    test_data data[] = {
            {"Test", 1, 2},
            {"Vector", 3, 4},
            {"Please", 5, 6},
            {"Ignore", 7, 8}
    };

    int i;
    for (i = 0; i < vector_size(v2); i++) {
        ASSERT_TRUE(test_data_equals(vector_get(v2, i), &data[i]));
    }

    vector_remove(v, 3);
    vector_remove(v, 2);
    vector_remove(v, 1);
    ASSERT_I_EQUAL(vector_size(v), 1);
    vector_set(v, 0, "Hello");
    ASSERT_I_EQUAL(vector_size(v), 1);
    vector_add(v, "World!");
    ASSERT_I_EQUAL(vector_size(v), 2);

    ASSERT_S_EQUAL("Hello", vector_get(v, 0));
    ASSERT_S_EQUAL("World!", vector_get(v, 1));

    vector v3 = vector_init(0);
    ASSERT_TRUE(v3);

    for (i = 0; i < 10000; i++) {
        vector_add(v3, "TEST_STRING");
    }
    ASSERT_I_EQUAL(vector_size(v3), 10000);

    vector_free(v);
    vector_free(v2);
    vector_free(v3);
    return true;
}

internal int8_t
run_pref_test(void) {

    char *test_file = "/.sdl/unit/test/preference.config";

    preference prefs = read_pref(test_file);
    ASSERT_TRUE(prefs);

    if (!pref_empty(prefs)) {
        ASSERT_S_EQUAL("preferences", get_spref(prefs, "testing"));
        ASSERT_S_EQUAL("pref_test", get_spref(prefs, "another"));
        ASSERT_S_EQUAL("test!", get_spref(prefs, "third"));
    }

    put_spref(prefs, "testing", "preferences");
    put_spref(prefs, "another", "pref_test");
    put_spref(prefs, "third", "test!");
    ASSERT_TRUE(!pref_empty(prefs));

    write_pref(prefs);

    char file[50] = {};
    char *home = getenv("HOME");
    strcat(file, home);
    strcat(file, test_file);
    ASSERT_TRUE(access(file, F_OK) != -1);

    free_pref(prefs);
    return true;
}

internal int8_t
run_random_test(void) {
    int i;
    for (i = 0; i < 10000; i++) {
        int rand_int = (int) random_in_range(10, 30);
        ASSERT_TRUE(rand_int >= 10 && rand_int <= 30);
        double rand_double = random_double();
        ASSERT_TRUE(rand_double >= 0.0 && rand_double <= 1.0);
    }
    return true;
}

/*
 * Add test cases to this array
 */
global test_function tests[] = {
        run_list_test,
        run_itoa_test,
        run_map_test,
        run_vector_test,
        run_pref_test,
        run_pref_test,
        run_random_test
};

global char* test_names[] = {
        "List Test",
        "ITOA Test",
        "Map Test",
        "Vector Test",
        "Preference Write Test",
        "Preference Read Test",
        "Random Utilities Test"
};

internal int
run_all_tests(void) {
    double total_tests = count(tests);
    double passed = 0;

    int i;
    for (i = 0; i < total_tests; i++) {
        char start[50];
        LOG_YELLOW(test_names[i]);
        LOG_YELLOW("\nStarting Test...\t\t\t\t\t");
        int8_t result = tests[i]();
        if (result) {
            passed++;
            LOG_GREEN("PASSED!");
        } else {
            LOG_RED("FAILED!");
        }
        printf("\n");
    }

    double percent = passed / total_tests;
    char final[50] = {};
    sprintf(final, "\t\t%0.2f%% of Tests Passed!\n", percent * 100);
    printf("\n\n");
    printf("===============================================================\n");
    printf("Unit Test Results: ");
    if (percent >= ACCEPTABLE_THRESHOLD) {
        LOG_GREEN(final);
        printf("===============================================================\n");
        return EXIT_SUCCESS;
    } else {
        LOG_YELLOW(final);
        printf("===============================================================\n");
        return EXIT_FAILURE;
    }
}

int
main(int argc, char **argv) {
    return run_all_tests();
}