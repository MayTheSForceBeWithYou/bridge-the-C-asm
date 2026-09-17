#include <stdio.h>
#include <stdlib.h>

int sink;

int busy_plain(int n)
{
    /* TODO: loop with a plain local; finish by assigning to sink */
    (void)n;
    return 0;
}

int busy_volatile(int n)
{
    /* TODO: same shape with volatile int */
    (void)n;
    return 0;
}

int main(void)
{
    /* Asm shape is the real point; soft check that both entry points run. */
    int p = busy_plain(1000);
    int v = busy_volatile(1000);
    printf("%d %d sink=%d\n", p, v, sink);
    (void)p; (void)v;
    puts("ok");
    return 0;
}
