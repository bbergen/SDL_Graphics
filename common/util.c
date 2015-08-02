//
// Created by bryan on 8/1/15.
//

#include <string.h>
#include "util.h"
#include "common.h"

void
itoa(int n, char s[], int pad) {
    //ensure there is enough room in s including '/0'
    assert(count(s) - 1 >= pad)

    int i;
    int digit = 0;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
        digit++;
    } while ((n /= 10) > 0);

    while (pad > digit) {
        s[i++] = '0';
        pad--;
    }

    s[i] = '\0';
    reverse(s);
}

static void
reverse(char s[]) {
    int tmp;
    int j;

    for (int i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}