#include <stdio.h>
#include <stdlib.h>

/* Tiny program for nvim-dap breakpoint / step practice.
 * Fill classify so the harness expectations hold; then debug it. */

int classify(int x)
{
    /* TODO:
     *  x < 0  -> -1
     *  x == 0 ->  0
     *  x > 0  ->  1
     */
    (void)x;
    return 0;
}

int accum(int n)
{
    /* TODO: return 0+1+...+(n-1); if n<=0 return 0 */
    (void)n;
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
    expect_int("classify(-3)", classify(-3), -1);
    expect_int("classify(0)", classify(0), 0);
    expect_int("classify(7)", classify(7), 1);
    expect_int("accum(5)", accum(5), 10);
    puts("ok");
    return 0;
}
