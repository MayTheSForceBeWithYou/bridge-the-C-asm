#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { N = 1 << 22 };

static inline uint64_t rdtscp_now(void)
{
    unsigned aux; uint64_t rax, rdx;
    /* TODO: rdtscp */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

/* GIVEN — measure this at -O2; do not edit the algorithm, only maybe mark */
long naive_sum(const int *a, int n, int thresh)
{
    long s = 0;
    int i;
    for (i = 0; i < n; i++) {
        if (a[i] > thresh)
            s += a[i];
        else
            s += 0; /* keep a branchy shape */
    }
    return s;
}

long fast_sum(const int *a, int n, int thresh)
{
    /* TODO: beat naive_sum at -O2 on rdtscp for the same a/n/thresh */
    (void)a; (void)n; (void)thresh;
    return 0;
}

int main(void)
{
    int *a = malloc(N * sizeof(int));
    int i;
    long r0, r1;
    uint64_t t0, t1, c_naive, c_fast;
    for (i = 0; i < N; i++)
        a[i] = i & 255;
    /* warmup */
    r0 = naive_sum(a, N, 128);
    r1 = fast_sum(a, N, 128);
    t0 = rdtscp_now();
    r0 = naive_sum(a, N, 128);
    t1 = rdtscp_now();
    c_naive = t1 - t0;
    t0 = rdtscp_now();
    r1 = fast_sum(a, N, 128);
    t1 = rdtscp_now();
    c_fast = t1 - t0;
    printf("naive cycles=%llu sum=%ld\n", (unsigned long long)c_naive, r0);
    printf("fast  cycles=%llu sum=%ld\n", (unsigned long long)c_fast, r1);
    if (r0 != r1) {
        fprintf(stderr, "FAIL checksum mismatch\n");
        return 1;
    }
    if (c_fast >= c_naive) {
        fprintf(stderr, "FAIL fast not faster (re-run; improve fast_sum)\n");
        return 1;
    }
    puts("ok");
    free(a);
    return 0;
}
