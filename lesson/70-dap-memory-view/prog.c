#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

int main(void)
{
    struct Point pt;
    int arr[4];
    memset(&pt, 0, sizeof pt);
    memset(arr, 0, sizeof arr);

    fill_demo(&pt, arr, 4);

    expect_int("pt.x", pt.x, 0x11);
    expect_int("pt.y", pt.y, 0x22);
    expect_int("arr[0]", arr[0], 10);
    expect_int("arr[3]", arr[3], 40);
    if (strcmp(pt.tag, "ABC") != 0) {
        fprintf(stderr, "FAIL tag: '%s'\n", pt.tag);
        exit(1);
    }
    puts("ok");
    return 0;
}
