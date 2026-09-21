#include <stdio.h>
#include <stdlib.h>

long scratch(void);

void call_site(void)
{
    long v = scratch();
    if (v != 42) {
        fprintf(stderr, "FAIL scratch=%ld\n", v);
        exit(1);
    }
}

int main(void)
{
    call_site();
    puts("ok");
    return 0;
}
