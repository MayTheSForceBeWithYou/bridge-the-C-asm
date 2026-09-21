# 02 — Prologue / epilogue (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `void empty(void)` as a truly empty body (`{}`).
2. Call it from `main`. `make asm`, then `make O=2 asm` (copy both `.s` files aside).
3. Also `make disasm` and `make O=2 disasm`.
4. Answer the observe questions in `LESSON.md` → Check yourself using your listings.

## Done when

- You can point at prologue / epilogue instructions in the `-O0` `.s`.
- You can say what happened to `empty` and its `call` at `-O2`.
- You explained `push %rbp` / `mov %rsp,%rbp` in your own words (with verbose-asm
  comments as backup).

## Lookup

Flag spellings only: `man 1 gcc`, `man 1 objdump`, `info gcc`.
