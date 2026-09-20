# 01 — Hello pipeline (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. `make` — confirm `./hello` runs.
2. `make preprocess asm obj disasm` — produce `hello.i`, `hello.s`, `hello.o`, `hello.lst`.
3. Skim each artifact using the questions in `LESSON.md` → Check yourself.
4. Change the string in `hello.c`, rebuild the full pipeline, and note what changed in
   `.i` vs `.s` vs `.lst` (data vs structure).

## Done when

- You can point at each artifact and say what job produced it.
- You found `main` in `hello.lst` and identified the call toward libc.
- You verified your "what changes when the string changes" prediction against the files.

## Lookup

Flag spellings only: `man 1 gcc`, `man 1 objdump`, `man 1 readelf`, `man 5 elf`.