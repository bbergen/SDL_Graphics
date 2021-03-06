//
// Created by bryan on 8/1/15.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "common.h"

internal void
reverse(char s[]) {
    int tmp;
    int i;
    int j;

    for (i = 0, j = (int) strlen(s) - 1; i < j; i++, j--) {
        tmp = s[i];
        s[i] = s[j];
        s[j] = (char) tmp;
    }
}

void
itoa(int n, char buffer[], int pad) {

    int i;
    int sign;

    if ((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    do {
        buffer[i++] = (char) (n % 10 + '0');
    } while ((n /= 10) > 0);

    if (sign < 0) {
        buffer[i++] = '-';
    }

    while (i < pad) {
        buffer[i++] = '0';
    }

    buffer[i] = '\0';
    reverse(buffer);
}

void
error(const char*(*error_function)(void)) {
    fprintf(stderr, "Error: %s\n", error_function());
    exit(EXIT_FAILURE);
}
