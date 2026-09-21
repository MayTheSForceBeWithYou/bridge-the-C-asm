#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(void)
{
    uint32_t x = 0x12345678u;
    uint32_t y;
    unsigned char *p = (unsigned char *)&x;
    printf("bytes: %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);
    /* TODO: y = __builtin_bswap32(x); print; expect 0x78563412 */
    y = 0;
    if (y != 0x78563412u) {
        fprintf(stderr, "FAIL bswap got 0x%08x\n", y);
        return 1;
    }
    puts("ok");
    return 0;
}
