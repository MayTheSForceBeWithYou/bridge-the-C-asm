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

long add2_c(long a, long b)
{
    /* TODO: plain C add */
    (void)a; (void)b;
    return 0;
}

long add2_asm(long a, long b)
{
    long sum;
    /* TODO: asm volatile Extended Asm adding a and b into sum */
    sum = 0;
    (void)a; (void)b;
    return sum;
}

int main(void)
{
    expect_long("add2_c", add2_c(20, 22), 42);
    expect_long("add2_asm", add2_asm(20, 22), 42);
    puts("ok");
    return 0;
}
