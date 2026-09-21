#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uint32_t bits_union(float f)
{
    /* TODO */
    (void)f;
    return 0;
}

uint32_t bits_memcpy(float f)
{
    uint32_t u;
    /* TODO: memcpy(&u, &f, sizeof u); */
    (void)f;
    u = 0;
    return u;
}

int main(void)
{
    float f = -2.5f;
    uint32_t a = bits_union(f);
    uint32_t b = bits_memcpy(f);
    printf("union=0x%08x memcpy=0x%08x\n", a, b);
    if (a != b || a != 0xc0200000u) {
        fprintf(stderr, "FAIL\n");
        return 1;
    }
    puts("ok");
    return 0;
}
