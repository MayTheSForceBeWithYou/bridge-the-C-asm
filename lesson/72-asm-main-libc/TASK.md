# 72 — `main` in asm, linked against libc (practice)

Read `LESSON.md` first — especially alignment at `main`, `%al` for variadic
calls, and how to find `<main>:`. This file is only the lab.

## Build and run

1. Implement `main` in `hello_asm.s`: `puts(greeting)`, then
   `printf(fmt, greeting, 14)`, then return 0.
2. `make && make run`.
3. Break alignment on purpose (remove `push`/`sub`), rerun under `-O0`, note
   where it dies (`bt`).
4. `make disasm` and compare your prologue to exercise 01's `gcc -S` `main`.

## Expected stdout

```
hello from asm
hello from asm has 14 chars
```

Exit status 0.

## Done when

- Stdout matches; exit 0; you recorded the alignment-break observation
  (which frame faulted).

## Lookup

`man 3 puts`, `man 3 printf`, `man 1 as`, `man 1 gcc`, `info as`.
