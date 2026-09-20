#include <stdio.h>

int main(void)
{
    int total = 0;
    int i;

    for (i = 1; i <= 5; i++) {
        total += i;
    }

    printf("total=%d\n", total);
    return 0;
}
