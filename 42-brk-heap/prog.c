#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    void *old_brk;
    void *new_brk;
    /* TODO:
       old_brk = sbrk(0);
       grow with brk(old + 4096) or sbrk(4096);
       write a pattern through the new bytes; verify;
       print old/new; puts("ok");
       Optional: malloc(1<<20) and compare under strace. */
    (void)old_brk;
    (void)new_brk;
    fprintf(stderr, "TODO implement brk grow\n");
    return 1;
}
