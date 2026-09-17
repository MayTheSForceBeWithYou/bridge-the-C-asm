    .global _start
    .text

_start:
    /* TODO: argc at (%rsp), argv0 at 8(%rsp);
       strlen argv0; write(1, argv0, len); write newline; exit 0 */
    mov $60, %rax
    mov $1, %rdi
    syscall
