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

int helper(int x, int y)
{
    /* TODO: nontrivial so regs get used; return x+y+1 */
    (void)x; (void)y;
    return 0;
}

int main(void)
{
    expect_int("helper(20,21)", helper(20, 21), 42);
    puts("ok");
    return 0;
}
