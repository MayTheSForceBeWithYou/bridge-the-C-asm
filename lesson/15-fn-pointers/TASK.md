# 15 — Function pointers (practice)

Read `LESSON.md` first — especially **Loading and calling — decode the lines**.
This file is only the lab.

## Build and inspect

1. Define `inc`, `dec`, and `int apply(int (*fp)(int), int x);` that calls `fp(x)`.
2. From `main`, call `apply(inc, …)` and `apply(dec, …)`.
3. `make asm disasm` at `-O0` and `-O2` (copy aside between builds).

## Done when

- You found how the pointer is loaded and called at `-O0` (`call *%reg` or
  equivalent).
- You recorded whether `-O2` inlined `apply` or turned the call direct.

## Lookup

`man 1 gcc`, `man 1 objdump`.
