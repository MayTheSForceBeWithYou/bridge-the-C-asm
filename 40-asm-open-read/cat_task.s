    .global _start
    .text

_start:
    /* TODO: open "TASK.md", read loop, write to fd 1, close, exit 0
       Syscalls: open=2 read=0 write=1 close=3 exit=60
       open args: rdi=path rsi=flags rdx=mode
       Use a .bss or stack buffer. */
    mov $60, %rax
    mov $1, %rdi
    syscall

    .section .rodata
path:
    .asciz "TASK.md"
