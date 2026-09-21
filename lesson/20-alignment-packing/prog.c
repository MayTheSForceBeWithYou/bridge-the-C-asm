#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


static void __attribute__((unused)) expect_long(const char *what, long got, long want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %ld want %ld\n", what, got, want);
        exit(1);
    }
}

static void __attribute__((unused)) expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

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
    expect_int("read_normal", (int)read_normal(&n), (int)(1u + 0x22222222u + 3u));
    expect_int("read_packed", (int)read_packed(&k), (int)(1u + 0x22222222u + 3u));
    puts("ok");
    return 0;
}
