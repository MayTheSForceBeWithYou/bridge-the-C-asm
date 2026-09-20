# 53 — explicit ld line (practice)

Read `LESSON.md` first. This file is only the lab.

## Link by hand

1. `start.s` with `_start` that writes a short message and exits.
2. `as --64 -g -o start.o start.s`
3. Link with an explicit `ld` line (Makefile `explicit` is a sketch — edit until it works).
4. Compare to `gcc -nostdlib -v start.s -o prog`; document every flag you passed.

## Done when

- `./prog` prints and exits 0; notes list each `ld` flag's purpose.

## Lookup

`man 1 ld`, `info ld`, `man 1 as`, `man 1 gcc`, `man 2 write`, `man 2 exit`.
