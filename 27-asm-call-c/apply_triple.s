    .global apply_triple
    .type apply_triple, @function

apply_triple:
    /* TODO: call c_triple(x); return its result
       Watch stack alignment (16 bytes) before call. */
    xorq %rax, %rax
    ret
