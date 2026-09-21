# 64 — memcpy pun vs union (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `bits_memcpy` and `bits_union` for a float's bits.
2. `make O=2 asm disasm` — do they collapse to the same load?
3. Note union vs memcpy in your notes; print `ok`.

## Done when

- Both return identical bits; `-O2` asm essentially equivalent; `ok`.

## Lookup

`man 3 memcpy`, `man 1 gcc`, `man 1 objdump`.
