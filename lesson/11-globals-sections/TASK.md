# 11 — Globals and sections (practice)

Read `LESSON.md` first — especially **How to read one `nm` line** and **`static`
changes visibility, not "stackness"**. This file is only the lab.

## Build and inspect

1. Define a modified global, an uninitialized global, a file-scope `static`, a
   `const` global, and a local `static` inside a function.
2. Build, then run `nm prog`, `size prog`, `objdump -h prog`, `readelf -S prog`.
3. Map each symbol to `.data` / `.bss` / `.rodata` / `.text` and note `nm` letters.

## Done when

- You placed each object in a section with evidence.
- You explained how `static` symbols differ in `nm` from globals.
- You stated what the relevant `nm` type letters mean *here*.

## Lookup

`man 1 nm`, `man 1 size`, `man 1 objdump`, `man 1 readelf`, `man 5 elf`.
