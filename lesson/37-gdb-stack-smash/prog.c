#include <stdio.h>
#include <string.h>

/* Intentional overflow for gdb observation — do not "fix". */
void smash(void)
{
    char buf[8];
    /* TODO: after observing once, try filling with a recognizable pattern
       e.g. memset(buf, 0x41, 64) and re-inspect the stack in gdb. */
    memset(buf, 0x41, 64);
    puts("smash: wrote past buf (may crash on return)");
}

int main(void)
{
    smash();
    puts("returned somehow");
    return 0;
}
