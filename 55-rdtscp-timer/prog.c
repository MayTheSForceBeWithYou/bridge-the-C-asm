#include <stdio.h>
#include <stdint.h>

static inline uint64_t rdtscp_now(void)
{
    unsigned aux;
    uint64_t rax, rdx;
    /* TODO: asm volatile("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(aux)); */
    (void)aux;
    rax = 0;
    rdx = 0;
    return (rdx << 32) | rax;
}

int main(void)
{
    volatile uint64_t acc = 0;
    int trial;
    for (trial = 0; trial < 3; trial++) {
        uint64_t t0 = rdtscp_now();
        uint64_t i;
        for (i = 0; i < 10000000ull; i++)
            acc += i;
        uint64_t t1 = rdtscp_now();
        printf("trial %d cycles=%llu acc=%llu\n",
               trial, (unsigned long long)(t1 - t0),
               (unsigned long long)acc);
    }
    puts("ok");
    return 0;
}
