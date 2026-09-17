# 15 — Function pointers (Track A)

## Goal
See how a call through a function pointer looks in asm.

## Do
1. Define two small functions (`inc`, `dec`) and a helper
   `int apply(int (*fp)(int), int x);` that calls `fp(x)`.
2. From `main`, call `apply(inc, …)` and `apply(dec, …)`.
3. `make asm disasm` at `-O0` and `-O2`.

## Observe / answer
- How is the function pointer loaded and called (`call *%reg`)?
- At `-O2`, does `apply` get inlined? Does the indirect call become direct?

## Refs
`man 1 gcc`, `man 1 objdump`
