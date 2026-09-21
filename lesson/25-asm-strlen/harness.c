#include <stdio.h>
#include <stdlib.h>

long my_strlen(const char *s);

static void expect(const char *s, long want)
{
    long got = my_strlen(s);
    if (got != want) {
        fprintf(stderr, "FAIL my_strlen(\"%s\")=%ld want %ld\n", s, got, want);
        exit(1);
    }
}

int main(void)
{
    expect("", 0);
    expect("a", 1);
    expect("hello", 5);
    expect("c-and-asm", 9);
    puts("ok");
    return 0;
}
