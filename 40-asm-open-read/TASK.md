# 40 — asm open/read/close/write (Track D)

## Goal
Raw syscalls only (no libc): open this exercise's `TASK.md`, read it, write
the bytes to stdout, close, exit.

Syscall numbers (from `/usr/include/asm/unistd_64.h` on this machine):
`read=0`, `write=1`, `open=2`, `close=3`, `exit=60`.

## Do
1. Implement `_start` in `cat_task.s` using `syscall` only.
2. Open path `TASK.md` (relative). On failure, exit non-zero.
3. Loop `read` into a buffer, `write` to fd 1, until EOF; `close`; `exit 0`.
4. Build freestanding (`make`). Run `./cat_task` and confirm it prints this
   file's contents.
5. Optional: if `command -v strace`, run `strace -e open,openat,read,write,close ./cat_task`
   (hint: `sudo pacman -S strace` if missing). Else reason from the code.

## Success
`./cat_task` stdout matches `TASK.md`. `nm`/`readelf` show no libc.

## Refs
`man 2 syscall`, `man 2 syscalls`, `man 2 open`, `man 2 read`, `man 2 write`,
`man 2 close`, `man 2 exit`, `/usr/include/asm/unistd_64.h`, `ausyscall`,
`man 1 as`, `man 1 ld`
