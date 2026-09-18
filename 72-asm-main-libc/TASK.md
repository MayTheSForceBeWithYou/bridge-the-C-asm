# 72 — `main` in asm, linked against libc (Track B)

## Goal
Write the whole `main` by hand and let `gcc` link it with the C runtime. No C
file in this exercise — the startup code, libc, and exit path are all still
there, and you have to meet their contract.

## Contract
- Provide a global symbol `main`; the CRT calls it and uses your `%eax` as the
  process exit status.
- Call `puts` and `printf` from libc. They are declared nowhere here; the
  linker resolves them because `gcc` links `-lc` by default.

## Do
1. Implement `main` in `hello_asm.s`: `puts(greeting)`, then
   `printf(fmt, greeting, 14)`, then return 0.
2. `make && make run`.
3. Break the alignment on purpose: remove your `push`/`sub` and run again under
   `-O0`. Note where it dies and in which libc routine.
4. `make disasm` and compare your prologue to what `gcc -S` emits for a C
   `main` in exercise 01.

## Expected stdout
```
hello from asm
hello from asm has 14 chars
```
Exit status 0. The stub links and exits 0 silently until you implement it.

## Observe / answer
- What is `%rsp % 16` at the first instruction of `main`, and why is it not 0?
  (Count what `call` pushed.)
- Which register tells a variadic function how many arguments are in vector
  registers? What happens to `printf` when you leave it garbage?
- Who calls `main`, and where does your return value go next?
  (`gdb -q ./hello_asm`, `break main`, `bt`.)

## Refs
`man 3 puts`, `man 3 printf`, `man 1 as`, `man 1 gcc`, `info as`.
Discover argument registers from your own exercise 04–05 `.s` output.
