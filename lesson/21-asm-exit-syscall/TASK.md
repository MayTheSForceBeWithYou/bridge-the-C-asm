# 21 — `_start` + exit syscall (practice)

Read `LESSON.md` first — especially the syscall register table and **How to read
one `objdump` line** for `_start`. This file is only the lab.

## Build and run

1. Fill `exit_n.s` so the process exits with status `42` (or another `N` you choose)
   via raw `__NR_exit` / `syscall`.
2. `make` then `make run` (prints `exit=...`) or `./exit_n; echo $?`.
3. Confirm with `readelf -h exit_n` and `objdump -d exit_n` (search `<_start>:`).

## Done when

- Process exit status matches the value you placed in `%rdi`.
- Disassembly of `_start` shows syscall setup (`mov` into `%rax` / `%rdi`, then
  `syscall`), not a libc `call exit` and not a relied-upon `ret`.
- Entry point in `readelf -h` matches `<_start>:`'s address.

## Lookup

Flag and number spellings only: `man 2 syscall`, `man 2 syscalls`, `man 2 exit`,
`man 1 as`, `man 1 ld`, `man 5 elf`, `/usr/include/asm/unistd_64.h`, `ausyscall`.
