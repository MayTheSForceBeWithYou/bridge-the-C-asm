#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

struct Node {
    int val;
    struct Node *next;
};

static unsigned char *arena_base;
static unsigned char *arena_bump;
static size_t arena_size;

void arena_init(size_t n)
{
    /* TODO: mmap n bytes; set base/bump/size */
    (void)n;
}

void *arena_alloc(size_t n)
{
    /* TODO: bump allocate; return NULL if OOM */
    (void)n;
    return NULL;
}

int list_sum(const struct Node *head)
{
    /* TODO */
    (void)head;
    return 0;
}

int main(void)
{
    /* TODO: arena_init; alloc three nodes 1->2->3; sum==6; munmap; puts("ok") */
    fprintf(stderr, "TODO bump arena list\n");
    return 1;
}
