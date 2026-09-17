#include <stdio.h>

int exported_add(int a, int b)
{
    return a + b;
}

static int hidden_helper(int x)
{
    return x + 1;
}

int main(void)
{
    printf("%d\n", exported_add(20, hidden_helper(21)));
    return 0;
}
