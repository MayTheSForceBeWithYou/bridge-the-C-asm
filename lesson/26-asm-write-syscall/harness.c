#include <stdio.h>
#include <string.h>
#include <unistd.h>

long my_write(int fd, const void *buf, unsigned long n);

int main(void)
{
    const char *s = "harness-write\n";
    long n = my_write(1, s, strlen(s));
    if (n < 0) {
        fprintf(stderr, "FAIL syscall returned %ld\n", n);
        return 1;
    }
    if ((unsigned long)n != strlen(s)) {
        fprintf(stderr, "FAIL short write %ld\n", n);
        return 1;
    }
    return 0;
}
