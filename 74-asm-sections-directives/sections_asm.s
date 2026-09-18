/* Place every object by hand: no compiler decides a section for you here. */

    .section .rodata
    /* TODO: a string constant `label_fmt` for "%s counter=%ld total=%ld\n" */
title:
    .asciz "sections"

    .data
    .align 8
    .global counter
    .type counter, @object
counter:
    .quad 7
    .size counter, 8

    /* TODO: an 8-byte initialized `step` in .data, value 5, globl + type + size */

    .bss
    /* TODO: a 64-byte zero-initialized `scratch` in .bss, 16-byte aligned */

    .text
    .global main
    .type main, @function
main:
    subq $8, %rsp
    /* TODO: total = counter + step; store it in the first 8 bytes of scratch,
       then printf(label_fmt, title, counter, total) and return 0. */
    xorl %eax, %eax
    addq $8, %rsp
    ret
    .size main, .-main
