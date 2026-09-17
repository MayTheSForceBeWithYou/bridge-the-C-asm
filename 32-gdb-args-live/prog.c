#include <stdio.h>
#include <stdlib.h>

int sum_idx(const int *a, int n)
{
    /* TODO: sum with a[i] */
    (void)a; (void)n;
    return 0;
}

int sum_ptr(const int *a, int n)
{
    /* TODO: sum with pointer walking */
    (void)a; (void)n;
    return 0;
}

static void expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

int main(void)
{
    int a[] = {1, 2, 3, 4, 5};
    expect_int("sum_idx", sum_idx(a, 5), 15);
    expect_int("sum_ptr", sum_ptr(a, 5), 15);
    puts("ok");
    return 0;
}
