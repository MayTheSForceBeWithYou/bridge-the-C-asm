#include <stdio.h>
#include <stdint.h>

static inline uint64_t rdtscp_now(void)
{
    unsigned aux;
    uint64_t rax, rdx;
    /* TODO: rdtscp into rax/rdx (copy from 55 once done) */
    (void)aux; rax = 0; rdx = 0;
    return (rdx << 32) | rax;
}

uint64_t work(uint64_t n)
{
    /* TODO: dependent accumulation so -O2 cannot delete the loop */
    (void)n;
    return 0;
}

int main(void)
{
    uint64_t t0 = rdtscp_now();
    uint64_t r = work(50000000ull);
    uint64_t t1 = rdtscp_now();
    printf("cycles=%llu result=%llu\n",
           (unsigned long long)(t1 - t0), (unsigned long long)r);
    puts("ok");
    return 0;
}
