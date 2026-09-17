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

int work(void)
{
    /* TODO: several locals; make sure each is used so -O0 keeps them. */
    return 0;
}

int main(void)
{
    /* Contract: work() returns sum of locals a=1,b=2,c=3,arr={4,5,6,7} => 28.
       Harness exits 0 / prints ok when behavior is correct. */
    expect_int("work()", work(), 28);
    puts("ok");
    return 0;
}
