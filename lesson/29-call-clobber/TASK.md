# 29 — Call clobbers (practice)

Read `LESSON.md` first — especially **How to read caller saves in asm** and **gdb
confirmation**. This file is only the lab.

## Build and inspect

1. Write `main` that keeps distinctive values live across a `call` (helper or
   libc). Inspect `-O0` asm for pre-call saves / post-call reloads — and/or use
   gdb `info registers` before and after the `call`.
2. Optionally inspect a callee prologue for pushes of preserved regs.

## Done when

- You listed registers not preserved across the call (caller-saved / clobbered)
  from *your* evidence.
- You listed registers a callee must restore if used (callee-saved).
- You did not rely on an ABI PDF.

## Lookup

`man 1 gcc`, `man 1 objdump`, `man 1 gdb`, gdb `help info registers`.
