    .global _start
    .global my_marker
    .text

_start:
    /* TODO: exit 0 (and optionally touch my_marker) */
    mov $60, %rax
    xor %rdi, %rdi
    syscall

    .section .mysec, "aw"
my_marker:
    .quad 0x1122334455667788
