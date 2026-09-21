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

int dense(int x)
{
    /* TODO: switch 0..7 returning distinct values; default -1 */
    (void)x;
    return -1;
}

int sparse(int x)
{
    /* TODO: switch on sparse cases; default -1 */
    (void)x;
    return -1;
}

int main(void)
{
    /* dense: x*x for 0..7 else -1; sparse: identity on {0,10,100,1000} else -1 */
    expect_int("dense(3)", dense(3), 9);
    expect_int("dense(9)", dense(9), -1);
    expect_int("sparse(100)", sparse(100), 100);
    expect_int("sparse(50)", sparse(50), -1);
    puts("ok");
    return 0;
}
