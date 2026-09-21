#include <stdio.h>
#include <stdlib.h>


static void __attribute__((unused)) expect_long(const char *what, long got, long want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %ld want %ld\n", what, got, want);
        exit(1);
    }
}

static void __attribute__((unused)) expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

long sum_for(long n)
{
    /* TODO: for-loop sum 1..n (define 0 if n<1) */
    (void)n;
    return 0;
}

long sum_while(long n)
{
    /* TODO: while-loop */
    (void)n;
    return 0;
}

long sum_do(long n)
{
    /* TODO: do-while — careful with n<1 */
    (void)n;
    return 0;
}

int main(void)
{
    expect_long("sum_for", sum_for(10), 55);
    expect_long("sum_while", sum_while(10), 55);
    expect_long("sum_do", sum_do(10), 55);
    puts("ok");
    return 0;
}
