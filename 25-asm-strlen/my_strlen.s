    .global my_strlen
    .type my_strlen, @function

my_strlen:
    /* TODO: pointer in first arg reg; return length in %rax */
    xorq %rax, %rax
    ret
