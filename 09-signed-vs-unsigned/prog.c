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

int cmp_signed(int a, int b)
{
    /* TODO: return -1 / 0 / 1 */
    (void)a; (void)b;
    return 0;
}

int cmp_unsigned(unsigned a, unsigned b)
{
    /* TODO: return -1 / 0 / 1 */
    (void)a; (void)b;
    return 0;
}

int main(void)
{
    expect_int("cmp_signed(-1,1)", cmp_signed(-1, 1), -1);
    expect_int("cmp_signed(1,1)", cmp_signed(1, 1), 0);
    expect_int("cmp_signed(2,1)", cmp_signed(2, 1), 1);
    expect_int("cmp_unsigned(0,1)", cmp_unsigned(0u, 1u), -1);
    expect_int("cmp_unsigned(1,1)", cmp_unsigned(1u, 1u), 0);
    puts("ok");
    return 0;
}
