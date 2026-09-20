# 49 — PIE vs -no-pie / ASLR (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and observe

1. `make`; also `gcc -ggdb -O0 -no-pie -o prog.nopie prog.c`.
2. `readelf -h` on both; record `Type:`.
3. gdb on PIE: break `main`, two `run`s comparing `$rip` / mappings; repeat with
   `set disable-randomization on`.

## Done when

- You recorded DYN vs EXEC and saw ASLR move the PIE base (unless disabled).

## Lookup

`man 1 readelf`, `man 5 elf`, `man 1 gdb`, `man 1 gcc`.
