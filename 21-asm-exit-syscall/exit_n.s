    .global _start
    .type _start, @function

_start:
    /* TODO: mov $60, %rax  (or whatever __NR_exit is)
             mov $42, %rdi  (exit status)
             syscall
       No ret — process should be gone after syscall. */
    ud2
