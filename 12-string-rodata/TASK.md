# 12 — String literals in `.rodata` (Track A)

## Goal
Find where string literals live and how they are referenced.

## Do
1. Write a program with several string literals (some identical, some used once,
   maybe a `char []` mutable copy vs `const char *`).
2. `make asm obj disasm`, then `objdump -s -j .rodata prog` (and/or `readelf -x .rodata`).

## Observe / answer
- Are identical literals merged?
- How does a `printf("...")` reference the bytes — RIP-relative? Absolute?
- What changes if you put the characters in a mutable `char buf[] = "...";`?

## Refs
`man 1 objdump`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`
