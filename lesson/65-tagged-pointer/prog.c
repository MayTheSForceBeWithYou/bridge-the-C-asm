#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uintptr_t tag_ptr(void *p, unsigned tag)
{
    /* TODO: tag in low 3 bits; tag must be 0..7 */
    (void)p; (void)tag;
    return 0;
}

void *untag_ptr(uintptr_t v)
{
    /* TODO */
    (void)v;
    return NULL;
}

unsigned get_tag(uintptr_t v)
{
    /* TODO */
    (void)v;
    return 0;
}

int main(void)
{
    int *p = malloc(sizeof(int));
    uintptr_t tagged;
    *p = 7;
    tagged = tag_ptr(p, 5);
    if (get_tag(tagged) != 5 || untag_ptr(tagged) != (void *)p) {
        fprintf(stderr, "FAIL tag roundtrip\n");
        return 1;
    }
    if (*((int *)untag_ptr(tagged)) != 7) {
        fprintf(stderr, "FAIL pointee\n");
        return 1;
    }
    puts("ok");
    free(p);
    return 0;
}
