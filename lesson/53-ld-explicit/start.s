    .global _start
    .text

_start:
    /* TODO: write(1, msg, len); exit(0) */
    mov $60, %rax
    xor %rdi, %rdi
    syscall

    .section .rodata
msg:
    .ascii "explicit-ld\n"
    .equ msg_len, . - msg
