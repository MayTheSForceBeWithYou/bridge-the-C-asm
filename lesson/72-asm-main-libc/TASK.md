# 72 — `main` in asm, linked against libc (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. Implement `main` in `hello_asm.s`: `puts(greeting)`, then
   `printf(fmt, greeting, 14)`, then return 0.
2. `make && make run`.
3. Break alignment on purpose (remove `push`/`sub`), rerun under `-O0`, note
   where it dies.
4. `make disasm` and compare your prologue to exercise 01's `gcc -S` `main`.

## Expected stdout

```
hello from asm
hello from asm has 14 chars
```

Exit status 0.

## Observe

- `%rsp % 16` at first instruction of `main` — why not 0?
- Role of `%al` for variadic calls; what if it is garbage?
- Who calls `main`? (`gdb -q ./hello_asm`, `break main`, `bt`.)

## Done when

- Stdout matches; exit 0; you recorded the alignment-break observation.

## Lookup

`man 3 puts`, `man 3 printf`, `man 1 as`, `man 1 gcc`, `info as`.
