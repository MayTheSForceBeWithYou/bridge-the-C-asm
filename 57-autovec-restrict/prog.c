#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { N = 1024 };

void add_arrays(float *dst, const float *a, const float *b, int n)
{
    /* TODO: dst[i] = a[i] + b[i] */
    (void)dst; (void)a; (void)b; (void)n;
}

void add_hazard(float *a, float *b, int n)
{
    /* TODO: a[i] = a[i] + b[i] — compiler must assume possible overlap */
    (void)a; (void)b; (void)n;
}

void add_restrict(float * restrict a, const float * restrict b, int n)
{
    /* TODO: same as hazard but with restrict */
    (void)a; (void)b; (void)n;
}

int main(void)
{
    float *a = calloc(N, sizeof(float));
    float *b = calloc(N, sizeof(float));
    float *d = calloc(N, sizeof(float));
    int i;
    for (i = 0; i < N; i++) { a[i] = 1.0f; b[i] = 2.0f; }
    add_arrays(d, a, b, N);
    add_hazard(a, b, N);
    add_restrict(a, b, N);
    if (d[0] < 2.99f || d[0] > 3.01f) {
        fprintf(stderr, "FAIL add_arrays\n");
        return 1;
    }
    puts("ok");
    free(a); free(b); free(d);
    return 0;
}
