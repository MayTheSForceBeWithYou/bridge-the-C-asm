    .global add3
    .type add3, @function

add3:
    /* TODO: a in first arg reg, b in second, c in third; sum → %rax */
    xorq %rax, %rax
    ret
