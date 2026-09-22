# 01 — Hello pipeline (practice)

Read `LESSON.md` first — especially **How to read one `objdump -d` line** and the worked
example. This file is only the lab.

## Build and inspect

1. `make` — confirm `./hello` runs.
2. `make preprocess asm obj disasm` — produce `hello.i`, `hello.s`, `hello.o`, `hello.lst`.
3. Work the Check yourself questions from the lesson against those files.
4. Change only the string in `hello.c`, rebuild the full pipeline, and note what changed in
   `.i` vs `.s` vs `.lst` (data vs structure).

## Reading `hello.lst`

Search for `<main>:`. Confirm `objdump -d hello` matches `hello.lst` (`make disasm` only
saves that listing). On the `call` toward `printf@plt` inside `main`, write down the
left-column instruction address and the target annotation.

## Done when

- You can point at each artifact and say what job produced it.
- You can decode one `objdump` line into address / bytes / mnemonic / target without
  mixing up RIP offsets.
- You found `<main>:` and identified stack setup vs the libc `call`.
- You verified your "what changes when the string changes" prediction against the files.

## Lookup

Flag spellings only: `man 1 gcc`, `man 1 objdump`, `man 1 readelf`, `man 5 elf`.
