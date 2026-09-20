#include <stdio.h>
#include <stdint.h>
#include "../test/check.h"

struct Normal {
    uint8_t a;
    uint32_t b;
    uint8_t c;
};

struct Packed {
    uint8_t a;
    uint32_t b;
    uint8_t c;
} __attribute__((packed));

uint32_t read_normal(const struct Normal *p)
{
    /* TODO: return combination of fields */
    (void)p;
    return 0;
}

uint32_t read_packed(const struct Packed *p)
{
    /* TODO */
    (void)p;
    return 0;
}

int main(void)
{
    struct Normal n = {1, 0x22222222u, 3};
    struct Packed k = {1, 0x22222222u, 3};
    printf("sizeof N=%zu P=%zu\n", sizeof n, sizeof k);
    /* Contract: return a+b+c */
    CHECK_EQ(read_normal(&n), 1u + 0x22222222u + 3u);
    CHECK_EQ(read_packed(&k), 1u + 0x22222222u + 3u);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
