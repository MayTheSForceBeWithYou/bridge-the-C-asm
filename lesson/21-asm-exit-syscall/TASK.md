# 21 — `_start` + exit syscall (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. Fill `exit_n.s` so the process exits with status `42` (or another `N` you choose)
   via raw `__NR_exit` / `syscall`.
2. `make` then `make run` (prints `exit=...`) or `./exit_n; echo $?`.
3. Confirm with `readelf -h exit_n` and `objdump -d exit_n`.

## Observe

- Why is there no `ret` from `_start`?
- What does `ld` need that `gcc` normally supplies?

## Done when

- Process exit status matches the value you placed in `%rdi`.
- Disassembly of `_start` shows syscall setup, not a libc `call exit`.

## Lookup

`man 2 syscall`, `man 2 syscalls`, `man 2 exit`, `man 1 as`, `man 1 ld`,
`man 5 elf`, `/usr/include/asm/unistd_64.h`, `ausyscall`.
