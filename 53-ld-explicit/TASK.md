# 53 — explicit ld line (Track E)

## Goal
Link a nostdlib `_start` with an explicit `ld` command you write — explain
each flag via `man 1 ld` / `info ld`.

## Do
1. Write `start.s` with `_start` that `write`s a short message and `exit`s.
2. Assemble: `as --64 -g -o start.o start.s`
3. Link with an explicit line (Makefile `explicit` target is a starting
   sketch — edit flags until it works), e.g.:
```
ld -o prog start.o
```
4. Compare to `gcc -nostdlib -v start.s -o prog` verbose dump — which ld
   flags did gcc hide?
5. Document every flag you passed.

## Success
`./prog` prints and exits 0. Your notes list each `ld` flag's purpose.

## Refs
`man 1 ld`, `info ld`, `man 1 as`, `man 1 gcc`, `man 2 write`, `man 2 exit`
