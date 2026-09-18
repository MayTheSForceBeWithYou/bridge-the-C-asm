    .section .rodata
msg:
    .asciz "addressed without a fixed load address"

    .text
    .global main
    .type main, @function

main:
    subq $8, %rsp
.ifdef USE_ABS
    /* Absolute address baked into the instruction. Assembles fine; the link
       is where it goes wrong for a position-independent executable. */
    movl $msg, %edi
.else
    /* TODO: load the address of `msg` into %rdi position-independently,
       i.e. as an offset from the current instruction pointer. */
    xorl %edi, %edi
.endif
    call puts
    xorl %eax, %eax
    addq $8, %rsp
    ret

    .size main, .-main
