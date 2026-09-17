# 02 — Prologue / epilogue (Track A)

## Goal
See what a nearly-empty function costs at `-O0` vs `-O2`.

## Do
1. Implement `void empty(void)` as a truly empty body (just `{}`).
2. `make asm` then `make O=2 asm` (save/compare both `.s` files — copy aside).
3. Also `make disasm` and `make O=2 disasm`.

## Observe / answer
- At `-O0`, what instructions open and close `empty`? What about `main`?
- At `-O2`, does `empty` still exist? Is it inlined? Eliminated?
- What do `push %rbp` / `mov %rsp,%rbp` accomplish? Confirm by reading the `.s`
  comments from `-fverbose-asm`.

## Refs
`man 1 gcc` (`-O`, `-S`, `-fverbose-asm`), `man 1 objdump`, `info gcc`
