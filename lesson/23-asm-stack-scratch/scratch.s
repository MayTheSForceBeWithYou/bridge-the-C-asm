    .global scratch_sum
    .type scratch_sum, @function

scratch_sum:
    /* TODO: frame pointer optional; allocate locals; store args;
             reload; return sum in %rax; tear down frame. */
    xorq %rax, %rax
    ret
