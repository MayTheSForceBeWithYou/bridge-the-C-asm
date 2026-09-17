    .global sum_to
    .type sum_to, @function

sum_to:
    /* TODO: sum 1..n in %rax; handle n==0 */
    xorq %rax, %rax
    ret
