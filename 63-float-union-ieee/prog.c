#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

union FloatBits {
    float f;
    uint32_t u;
};

uint32_t bits_of(float f)
{
    /* TODO: union pun; return raw bits */
    (void)f;
    return 0;
}

int main(void)
{
    /* TRACE on paper first!
       Expected after correct impl: 1.0f -> 0x3f800000, -2.5f -> 0xc0200000 */
    uint32_t b1 = bits_of(1.0f);
    uint32_t b2 = bits_of(-2.5f);
    printf("1.0f -> 0x%08x\n", b1);
    printf("-2.5f -> 0x%08x\n", b2);
    if (b1 != 0x3f800000u || b2 != 0xc0200000u) {
        fprintf(stderr, "FAIL unexpected bits (finish TODO / check TRACE)\n");
        return 1;
    }
    puts("ok");
    return 0;
}
