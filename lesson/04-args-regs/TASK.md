# 04 — Argument registers (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `f1`, `f2`, `f6`, and `f7` as in the lesson goal (simple mixes/sums of
   `long` args). Call each from `main` with distinct constants.
2. `make asm disasm`.
3. From the listings only, record registers for args 1–6, the home of arg 7, and
   where the caller reads the return.

## Done when

- You can list args 1–6 registers from *your* asm (not from memory of a handout).
- You showed where the seventh argument lives.
- You identified the return-value register the caller uses.

## Lookup

`man 1 gcc`, `man 1 objdump` — discover ABI from output only.
