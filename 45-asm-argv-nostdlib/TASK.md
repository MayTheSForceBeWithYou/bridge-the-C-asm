# 45 — argv from initial stack (Track D)

## Goal
Freestanding `_start`: read `argc`/`argv` from the kernel stack layout, write
`argv[0]` to stdout via raw `write`, exit.

Initial stack (x86-64 Linux): at entry `%rsp` points at `argc`, then
`argv[0]..argv[argc]` (NULL), then `envp...`.

## Do
1. Implement `_start` in `argv0.s`.
2. Load argc from `(%rsp)`, load argv0 pointer from `8(%rsp)`.
3. Compute length of argv0 string; `write(1, argv0, len)`.
4. `write` a trailing newline; `exit 0`.
5. Run `./argv0` and `./argv0 foo` — first printed token is the program path
   / first arg as launched.

## Success
Stdout starts with argv[0] text. No libc (`as`+`ld`).

## Refs
`man 2 write`, `man 2 exit`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`
