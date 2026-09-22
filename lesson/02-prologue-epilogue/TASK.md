# 02 — Prologue / epilogue (practice)

Read `LESSON.md` first — especially **Frame pointer setup at `-O0`**, **What
`-O2` is allowed to do**, and the worked example. This file is only the lab.

## Build and inspect

1. Implement `void empty(void)` as a truly empty body (`{}`).
2. Call it from `main`. `make asm`, then *copy* `prog.s` aside (for example
   `cp prog.s prog-O0.s`) before `make O=2 asm`.
3. Also `make disasm` and `make O=2 disasm` (copy `.lst` aside the same way if
   you need both).
4. Work the Check yourself questions from the lesson against those listings.

## Reading the listings

- In `.s`, search `empty:` / `main:`; skip `.cfi_*`.
- In `.lst`, search `<empty>:` / `<main>:`.
- On the `-O0` `empty` body, point at prologue vs epilogue instructions.
- At `-O2`, record what happened to `empty` and to the `call` in `main`.

## Done when

- You can point at prologue / epilogue instructions in the `-O0` `.s`.
- You can say what happened to `empty` and its `call` at `-O2`.
- You explained `push %rbp` / `mov %rsp,%rbp` in your own words (with verbose-asm
  comments as backup).
- You did not confuse `.cfi_*` lines with CPU instructions.

## Lookup

Flag spellings only: `man 1 gcc`, `man 1 objdump`, `info gcc`.
