#include <stdio.h>
#include <stdlib.h>


static void __attribute__((unused)) expect_long(const char *what, long got, long want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %ld want %ld\n", what, got, want);
        exit(1);
    }
}

static void __attribute__((unused)) expect_int(const char *what, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", what, got, want);
        exit(1);
    }
}

struct Node {
    int val;
    struct Node *next;
};

int list_sum(const struct Node *head)
{
    /* TODO: walk and sum val */
    (void)head;
    return 0;
}

int main(void)
{
    struct Node c = {3, NULL};
    struct Node b = {2, &c};
    struct Node a = {1, &b};
    expect_int("list_sum", list_sum(&a), 6);
    puts("ok");
    return 0;
}
