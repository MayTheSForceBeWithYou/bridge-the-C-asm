# 13 — Recursion and stack growth (practice)

Read `LESSON.md` first — especially **gdb as a ruler** and **Estimating stride
from disassembly alone**. This file is only the lab.

## Build and inspect

1. Implement recursive `long fac(long n);` (non-tail form is fine).
2. `make` at `-O0`, then use gdb:
   `break fac`, `run`, `stepi`, `info registers rsp rbp`, and `disassemble fac`.
3. Step through several recursive entries and record `%rsp` movement.

## Done when

- You stated how much `%rsp` moves per call (with evidence).
- You listed what is saved each entry (return address, `%rbp`, locals).
- You matched that list to `disassemble fac`.

## Lookup

`man 1 gdb`, gdb help for `disassemble` / `stepi` / `info registers`, `man 1 objdump`.
