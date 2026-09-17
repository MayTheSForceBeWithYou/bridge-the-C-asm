    .global _start
    .type _start, @function

    .section .rodata
msg:
    .ascii "hello from write syscall\n"
msg_end:

    .text
_start:
    /* TODO: write(1, msg, len) then exit(0) via syscall */
    ud2
