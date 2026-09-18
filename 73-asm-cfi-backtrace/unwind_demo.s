    .bss
    .align 16
frames:
    .space 128

    .section .rodata
count_fmt:
    .asciz "frames captured: %d\n"

    .text

/* No function here touches %rbp, and both callers move %rsp by an amount that
   is only known at run time. A debugger reading the prologue cannot work out
   the frame size, so unwinding depends entirely on the call-frame information
   you write by hand. */

    .global level2
    .type level2, @function
level2:
    /* TODO: .cfi_startproc here, and keep .cfi_def_cfa_offset in step with
       every change to %rsp below. .cfi_endproc after the ret. */
    movq $56, %rax
    subq %rax, %rsp
    leaq frames(%rip), %rdi
    movl $16, %esi
    call backtrace
    movl %eax, 8(%rsp)
    leaq count_fmt(%rip), %rdi
    movl 8(%rsp), %esi
    xorl %eax, %eax
    call printf
    leaq frames(%rip), %rdi
    movl 8(%rsp), %esi
    movl $1, %edx
    call backtrace_symbols_fd
    movq $56, %rax
    addq %rax, %rsp
    ret
    .size level2, .-level2

    .global level1
    .type level1, @function
level1:
    /* TODO: same treatment. Note the frame size lives in %r11 at run time. */
    movq $24, %r11
    subq %r11, %rsp
    call level2
    movq $24, %r11
    addq %r11, %rsp
    ret
    .size level1, .-level1

    .global main
    .type main, @function
main:
    /* TODO: annotate this frame too — the outermost one you own. */
    subq $8, %rsp
    call level1
    xorl %eax, %eax
    addq $8, %rsp
    ret
    .size main, .-main
