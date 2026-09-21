#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { Q = 16 };

int32_t q_from_int(int v)
{
    return (int32_t)v << Q;
}

int32_t q_mul(int32_t a, int32_t b)
{
    /* TODO: (int64_t)a * b >> Q */
    (void)a; (void)b;
    return 0;
}

int32_t q_div(int32_t a, int32_t b)
{
    /* TODO: ((int64_t)a << Q) / b */
    (void)a; (void)b;
    return 0;
}

int main(void)
{
    int32_t a = q_from_int(3) / 2; /* 1.5 */
    int32_t b = q_from_int(2);
    int32_t p = q_mul(a, b);
    int32_t d = q_div(q_from_int(3), q_from_int(2));
    if (p != q_from_int(3)) {
        fprintf(stderr, "FAIL mul got %d want %d\n", p, q_from_int(3));
        return 1;
    }
    if (d != a) {
        fprintf(stderr, "FAIL div\n");
        return 1;
    }
    puts("ok");
    return 0;
}
