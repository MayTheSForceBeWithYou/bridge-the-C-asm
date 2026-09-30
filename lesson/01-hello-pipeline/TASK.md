# 01 — Hello pipeline (practice)

Read `LESSON.md` first — especially **How to read one `objdump -d` line** and Step 5 of the
worked example. This file is only the lab.

## Build and inspect

1. `make` — confirm `./hello` runs.
2. `make preprocess asm obj disasm` — produce `hello.i`, `hello.s`, `hello.o`, `hello.lst`.
3. Work the Check yourself questions from the lesson against those files.
4. Copy baselines (`*.before`), change **only** the string literal in `hello.c`, rebuild
   with the default `CFLAGS` from `../../common.mk` (still `-O0` — do not pass `O=…`),
   and diff `.i` / `.s` / the `<main>:` region of `.lst` (and `cmp` the binary if you
   want). Record what moved: literal payload vs materialization vs frame/`call` shape.
   (`-O0` is a Make/`gcc` flag, not a string inside the artifacts; `make -n asm` shows it.)

## Reading `hello.lst`

Search for `<main>:`. Confirm `objdump -d hello` matches `hello.lst` (`make disasm` only
saves that listing). On the `call` toward `printf@plt` inside `main`, write down the
left-column instruction address and the target annotation.

## Done when

- You can name what each artifact encodes and which stage produces it.
- You can decode one `objdump` line into address / bytes / mnemonic / target without
  mixing up RIP offsets.
- You found `<main>:` and identified stack setup vs the libc `call`.
- For the string edit, you can state specifically what changed inside `.i`, `.s`, and
  `.lst`/`main` — not a vague "some files change."

## Lookup

Flag spellings only: `man 1 gcc`, `man 1 objdump`, `man 1 readelf`, `man 5 elf`.
