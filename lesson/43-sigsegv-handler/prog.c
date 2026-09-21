#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void on_segv(int sig)
{
    /* TODO: async-signal-safe report + _exit(0) */
    (void)sig;
}

int main(void)
{
    struct sigaction sa;
    /* TODO: sa.sa_handler = on_segv; sigemptyset; sigaction(SIGSEGV,&sa,NULL); */
    (void)sa;
    volatile int *p = NULL;
    /* after handler installed: */
    (void)p;
    /* *p = 1; */
    fprintf(stderr, "TODO install handler then deref NULL\n");
    return 1;
}
