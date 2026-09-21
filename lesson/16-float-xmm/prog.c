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

float addf(float a, float b)
{
    /* TODO */
    (void)a; (void)b;
    return 0.0f;
}

double muld(double a, double b)
{
    /* TODO */
    (void)a; (void)b;
    return 0.0;
}

double mix(double a, int n)
{
    /* TODO: e.g. a * n */
    (void)a; (void)n;
    return 0.0;
}

int main(void)
{
    float af = addf(1.5f, 2.5f);
    double md = muld(2.0, 3.0);
    double mx = mix(1.5, 4);
    if (af < 3.99f || af > 4.01f) {
        fprintf(stderr, "FAIL addf: got %f want ~4.0\n", (double)af);
        exit(1);
    }
    if (md < 5.99 || md > 6.01) {
        fprintf(stderr, "FAIL muld: got %f want ~6.0\n", md);
        exit(1);
    }
    if (mx < 5.99 || mx > 6.01) {
        fprintf(stderr, "FAIL mix: got %f want ~6.0\n", mx);
        exit(1);
    }
    puts("ok");
    return 0;
}
