    .global my_write
    .type my_write, @function

my_write:
    /* TODO: syscall write(fd, buf, n); return value in %rax */
    xorq %rax, %rax
    ret
