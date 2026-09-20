# 12 — String literals in `.rodata` (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Write a program with several string literals (some identical), plus a mutable
   `char buf[] = "...";` contrasted with `const char *`.
2. `make asm obj disasm`, then `objdump -s -j .rodata prog` and/or
   `readelf -x .rodata prog`.
3. Answer merging, addressing mode, and mutable-buffer questions from the lesson.

## Done when

- You found the literal bytes in `.rodata`.
- You described how `printf("...")` references them in asm.
- You explained what changes for a mutable `char buf[]` initializer.

## Lookup

`man 1 objdump`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`.
