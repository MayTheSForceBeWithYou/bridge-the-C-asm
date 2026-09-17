# 10 — Switch: jump table vs compares (Track A)

## Goal
See when GCC emits a jump table versus cascaded compares.

## Do
1. Implement `int dense(int x);` — `switch` on cases 0..7 (contiguous).
2. Implement `int sparse(int x);` — `switch` on cases like 0, 10, 100, 1000.
3. `make asm` at `-O0` and `-O2`.

## Observe / answer
- Which form gets a jump table (look for `.rodata` / `jmp *...` patterns)?
- How does the sparse form decide?
- What changes between `-O0` and `-O2`?

## Refs
`man 1 gcc`, `man 1 objdump`, `man 5 elf`
