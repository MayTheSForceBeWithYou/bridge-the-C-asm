# 08 — Loop jump shapes (practice)

Read `LESSON.md` first — especially **Reading a jump graph** and **Recognizing
test-at-top vs test-at-bottom**. This file is only the lab.

## Build and inspect

1. Implement three functions that sum `1..n` (or fold an array), each using
   `for`, `while`, or `do`/`while` exactly once.
2. `make asm` at `-O0`. Sketch the jump graph for each (real `.L*` names + your
   `test`/`body`/`done` aliases).
3. Optionally compare `make O=2 asm` for reshaping.

## Done when

- You marked test location (top vs bottom) for each function.
- You counted conditional vs unconditional jumps per loop.
- You noted whether `-O2` converged the shapes (if you built it).

## Lookup

`man 1 gcc`, `man 1 objdump`.
