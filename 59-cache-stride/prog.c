#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { BYTES = 64 * 1024 * 1024 };
enum { LINE = 64 }; /* L1D line size on this machine */

static inline uint64_t rdtscp_now(void)
{
    unsigned aux; uint64_t rax, rdx;
    /* TODO */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

int main(void)
{
    unsigned char *buf = malloc(BYTES);
    uint64_t t0, t1, acc = 0;
    size_t i;
    /* TODO: touch buf sequentially; time; then stride LINE; time; printf */
    (void)buf; (void)t0; (void)t1; (void)acc; (void)i;
    fprintf(stderr, "TODO implement stride timing\n");
    return 1;
}
