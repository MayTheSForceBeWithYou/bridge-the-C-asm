#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { N = 1 << 20 };

static inline uint64_t rdtscp_now(void)
{
    unsigned aux; uint64_t rax, rdx;
    /* TODO: rdtscp */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

long sum_thresh(const int *a, int n, int thresh)
{
    /* TODO: sum a[i] where a[i] > thresh */
    (void)a; (void)n; (void)thresh;
    return 0;
}

static int cmp_int(const void *x, const void *y)
{
    const int *a = x, *b = y;
    return (*a > *b) - (*a < *b);
}

int main(void)
{
    int *sorted = malloc(N * sizeof(int));
    int *shuffled = malloc(N * sizeof(int));
    int i;
    long c1, c2;
    uint64_t t0, t1;
    /* TODO: fill, qsort sorted, time both, printf cycles, verify equal sums */
    (void)sorted; (void)shuffled; (void)i; (void)c1; (void)c2; (void)t0; (void)t1;
    fprintf(stderr, "TODO implement\n");
    return 1;
}
