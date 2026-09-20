# 47 — static vs dynamic hello (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and compare

1. Tiny hello in `hello.c`; `make`; keep a dynamic copy.
2. Try `gcc -ggdb -static -o hello.static hello.c` (or `-static-pie` if needed). If both
   fail, document the error and analyze `hello.dyn` thoroughly.
3. Compare with `ldd`, `size`, `readelf -d` on each artifact you have.

## Done when

- You have a filled comparison table (dyn always; static if link succeeded).
- You can answer: who has dynamic section / INTERP, who is larger, why.

## Lookup

`man 1 ldd`, `man 1 size`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`.
