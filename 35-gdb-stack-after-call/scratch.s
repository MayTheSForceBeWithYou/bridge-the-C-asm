    .global scratch
    .type scratch, @function

scratch:
    /* TODO: push 3 regs, pop them, return 42 in %rax */
    mov $0, %rax
    ret
