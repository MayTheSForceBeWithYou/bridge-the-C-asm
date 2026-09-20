#include <stdio.h>
#include <stdlib.h>
#include "../test/check.h"

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
    CHECK_EQ(list_sum(&a), 6);
    if (test_report() != 0)
        return 1;
    puts("ok");
    return 0;
}
