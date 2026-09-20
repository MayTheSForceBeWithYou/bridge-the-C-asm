# 74 — Sections and symbols by hand (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. Fill TODOs in `sections_asm.s`: format string in `.rodata`, `step` in
   `.data`, `scratch` in `.bss`, and `main`.
2. `make && make run`.
3. Prove placement:
   ```
   nm -S --size-sort sections_asm | grep -Ei 'counter|step|scratch'
   readelf -S sections_asm
   size sections_asm
   objdump -s -j .data -j .rodata sections_asm
   ```
4. Move `scratch` into `.data` (`.space 64, 0`), re-run `size`, note the delta.

## Expected stdout

```
sections counter=7 total=12
```

## Observe

- `nm` letters for `counter` / `step` / `scratch`.
- Why `.bss` is cheap on disk but not in address space.
- Effect of dropping `.align` / `.balign`; of removing `.type` / `.size`.
- Writing through a `.rodata` object — which signal?

## Done when

- Stdout matches; you have `nm`/`size` evidence for section placement.

## Lookup

`info as` (Section, Data directives, Align), `man 1 nm`, `man 1 readelf`,
`man 1 size`, `man 1 objdump`, `man 5 elf`.
