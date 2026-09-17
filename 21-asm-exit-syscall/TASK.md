# 21 — `_start` + exit syscall (Track B)

## Goal
Freestanding program: no libc, exit with status `N` via raw syscall.

## Contract (Linux x86-64)
- Entry symbol: `_start` (not `main`)
- Syscall number in `%rax`
- Args in `%rdi`, `%rsi`, `%rdx`, `%r10`, `%r8`, `%r9` (for this exercise you
  only need the first)
- `__NR_exit` = 60 — confirm in `/usr/include/asm/unistd_64.h` or `ausyscall exit`
- Invoke with `syscall`
- Exit status = value in `%rdi`

## Do
1. Fill `exit_n.s` so the process exits with code `42` (or another `N` you choose).
2. `make` then `make run` (prints `exit=...`) or `./exit_n; echo $?`

## Observe / answer
- Why is there no `ret` from `_start`?
- What does `ld` need that `gcc` normally supplies? (`man 1 ld`, `man 1 as`)
- Confirm with `readelf -h exit_n` / `objdump -d exit_n`.

## Refs
`man 2 syscall`, `man 2 syscalls`, `man 2 exit`, `man 1 as`, `man 1 ld`,
`man 5 elf`, `/usr/include/asm/unistd_64.h`, `ausyscall`
