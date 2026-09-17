    .global _start
    .text

_start:
    /* TODO: open file, read into buffer, count newlines,
       write decimal count, exit. No libc. */
    mov $60, %rax
    mov $1, %rdi
    syscall
