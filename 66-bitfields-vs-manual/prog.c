#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct BF {
    unsigned a : 5;
    unsigned b : 7;
    unsigned c : 20;
};

uint32_t pack_manual(unsigned a, unsigned b, unsigned c)
{
    /* TODO: pack into 32 bits */
    (void)a; (void)b; (void)c;
    return 0;
}

unsigned get_a_manual(uint32_t v)
{
    /* TODO */
    (void)v;
    return 0;
}

int main(void)
{
    struct BF bf;
    uint32_t m;
    bf.a = 3; bf.b = 9; bf.c = 1000;
    m = pack_manual(3, 9, 1000);
    printf("sizeof BF=%zu manual=4 a_bf=%u a_man=%u\n",
           sizeof bf, (unsigned)bf.a, get_a_manual(m));
    if (get_a_manual(m) != 3) {
        fprintf(stderr, "FAIL\n");
        return 1;
    }
    puts("ok");
    return 0;
}
