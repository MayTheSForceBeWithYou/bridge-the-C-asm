# 11 — Globals and sections (Track A)

## Goal
Map C storage to ELF sections with `nm` / `size` / `objdump -h`.

## Do
1. Define:
   - a modified global (`int g = 1;`)
   - an uninitialized global (`int h;`)
   - a `static` file-scope variable
   - a `const` global string or int
   - a local `static` inside a function
2. Build, then run: `nm prog`, `size prog`, `objdump -h prog`, `readelf -S prog`.

## Observe / answer
- Which symbols are in `.data`, `.bss`, `.rodata`, `.text`?
- How do `static` symbols appear in `nm` vs globals?
- What do the `nm` type letters mean here? (`man 1 nm`)

## Refs
`man 1 nm`, `man 1 size`, `man 1 objdump`, `man 1 readelf`, `man 5 elf`
