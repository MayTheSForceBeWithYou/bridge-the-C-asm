# 01 — Hello pipeline (Track A)

## Goal
Run the full translation pipeline on a tiny program and know what each artifact is.

## Do
1. `make` (binary should already build from the starter).
2. `make preprocess asm obj disasm` — produce `hello.i`, `hello.s`, `hello.o`, `hello.lst`.
3. Skim each file. Then change the string in `hello.c`, rebuild the pipeline, and
   note what changed in `.i` vs `.s` vs `.lst`.

## Observe / answer
- What is left in `.i` that was not in `.c`? (`man 1 gcc` — look at `-E`)
- What section names appear around your string in `.s`?
- How does `hello.o` differ from the final `hello` binary? (`man 5 elf`, `man 1 objdump`, `readelf -h`)
- In `hello.lst`, find `main`. Which instructions call into libc?

## Refs
`man 1 gcc`, `man 5 elf`, `man 1 objdump`, `man 1 readelf`, `info gcc`
