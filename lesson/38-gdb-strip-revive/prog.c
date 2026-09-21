#include <stdio.h>
#include <stdlib.h>

int secret(void)
{
    /* TODO: return 42 */
    return 0;
}

int main(void)
{
    if (secret() != 42) {
        fprintf(stderr, "FAIL secret\n");
        exit(1);
    }
    puts("ok");
    return 0;
}
