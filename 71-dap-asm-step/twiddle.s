    .global twiddle
    .type twiddle, @function

twiddle:
    /* TODO: a=%rdi b=%rsi c=%rdx
     *  result = (a ^ b) + c in %rax
     *  e.g. xorq %rsi, %rdi ; leaq (%rdi,%rdx), %rax ; ret
     */
    xorq %rax, %rax
    ret
