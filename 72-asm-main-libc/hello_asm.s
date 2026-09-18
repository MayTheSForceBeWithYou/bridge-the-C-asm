    .section .rodata
greeting:
    .asciz "hello from asm"
fmt:
    .asciz "%s has %ld chars\n"

    .text
    .global main
    .type main, @function

main:
    /* TODO: 1) call puts(greeting)
             2) call printf(fmt, greeting, 14)
             3) return 0 in %eax
       Two ABI details bite here:
         - %rsp is 8 mod 16 on entry (the call to main pushed a return address);
           fix it before your own calls or libc's aligned SSE stores fault.
         - printf is variadic: %al must say how many vector regs hold args. */
    xorl %eax, %eax
    ret

    .size main, .-main
