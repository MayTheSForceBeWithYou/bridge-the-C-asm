#include <stdio.h>
#include <stdint.h>

static inline uint64_t rdtscp_now(void)
{
    unsigned aux; uint64_t rax, rdx;
    /* TODO */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

int main(void)
{
    volatile uint64_t acc = 0;
    uint64_t i, t0, t1;
    t0 = rdtscp_now();
    for (i = 0; i < 50000000ull; i++)
        acc += i;
    t1 = rdtscp_now();
    printf("cycles=%llu acc=%llu\n",
           (unsigned long long)(t1 - t0), (unsigned long long)acc);
    puts("ok");
    return 0;
}
