# 74 — Sections and symbols by hand (Track B)

## Goal
Put data in `.rodata`, `.data`, and `.bss` yourself with assembler directives,
then prove where each object landed. Exercise 11 read this layout out of
compiler output; here you author it.

## Do
1. Fill the TODOs in `sections_asm.s`: the format string in `.rodata`, `step`
   in `.data`, `scratch` in `.bss`, and the `main` body.
2. `make && make run`.
3. Prove the placement:
   ```
   nm -S --size-sort sections_asm | grep -Ei 'counter|step|scratch'
   readelf -S sections_asm
   size sections_asm
   objdump -s -j .data -j .rodata sections_asm
   ```
4. Move `scratch` from `.bss` into `.data` (initialize it with `.space 64, 0`)
   and re-run `size`. Keep the version you prefer; note the file-size delta.

## Expected stdout
```
sections counter=7 total=12
```

## Observe / answer
- Which symbol letter does `nm` give each of `counter`, `step`, `scratch`, and
  why does one differ?
- Why does `.bss` cost nothing in the file but still cost address space?
- What breaks if you drop `.align`/`.balign` before an 8-byte `.quad` or a
  16-byte buffer? Check with `nm -S` and an SSE store.
- What do `.type` and `.size` change? Try removing them and compare
  `readelf -s` output and a `gdb` `print counter`.
- Which of your objects would `.section .rodata` refuse to let you write to at
  runtime, and what signal do you get?

## Refs
`info as` (nodes: Section, Data directives, Align), `man 1 nm`, `man 1 readelf`,
`man 1 size`, `man 1 objdump`, `man 5 elf`.
