#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

enum { ITERS = 100000000 };

struct SharedTight {
    int a;
    int b;
};

struct SharedPadded {
    int a;
    char pad[60]; /* 4+60=64 */
    int b;
};

static inline uint64_t rdtscp_now(void)
{
    unsigned aux; uint64_t rax, rdx;
    /* TODO */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

/* TODO: thread funcs bumping a or b ITERS times; time tight vs padded */

int main(void)
{
    fprintf(stderr, "TODO false sharing harness\n");
    return 1;
}
