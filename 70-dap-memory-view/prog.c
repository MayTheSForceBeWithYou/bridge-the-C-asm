#include <stdio.h>
#include <string.h>
#include "../test/check.h"

struct Point {
    int x;
    int y;
    char tag[4];
};

/* Fill fill_demo so main's expectations hold. Then inspect bytes in dap. */
void fill_demo(struct Point *p, int *arr, int n)
{
    /* TODO:
     *  p->x = 0x11; p->y = 0x22;
     *  p->tag[0]='A'; p->tag[1]='B'; p->tag[2]='C'; p->tag[3]='\0';
     *  arr[i] = (i+1) * 10 for i in [0,n)
     */
    (void)p; (void)arr; (void)n;
}

int main(void)
{
    struct Point pt;
    int arr[4];
    memset(&pt, 0, sizeof pt);
    memset(arr, 0, sizeof arr);

    fill_demo(&pt, arr, 4);

    CHECK_EQ(pt.x, 0x11);
    CHECK_EQ(pt.y, 0x22);
    CHECK_EQ(arr[0], 10);
    CHECK_EQ(arr[3], 40);
    CHECK_STR(pt.tag, "ABC");
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
