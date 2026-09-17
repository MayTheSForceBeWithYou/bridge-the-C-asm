#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    /* Printable summary - rodata/string shape is the point. */
    const char *a = "TODO";
    char buf[] = "TODO";
    printf("len a=%zu buf=%zu\n", strlen(a), strlen(buf));
    if (strlen(a) == 0) {
        fprintf(stderr, "FAIL empty literal\n");
        exit(1);
    }
    puts("ok");
    return 0;
}
