# 40 — asm open/read/close/write (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. Write `_start` in `cat_task.s` using `syscall` only (no libc).
2. Open relative path `TASK.md`; on failure exit non-zero.
3. Loop `read` → `write` to fd 1 until EOF; `close`; `exit 0`.
4. Numbers on this machine: `read=0`, `write=1`, `open=2`, `close=3`, `exit=60`.
5. `make` and run `./cat_task`. Optional: `command -v strace && strace -e open,openat,read,write,close ./cat_task`.

## Done when

- `./cat_task` stdout matches `TASK.md`.
- `nm` / `readelf` show no libc dependency for your program.

## Lookup

`man 2 syscall`, `man 2 open`, `man 2 read`, `man 2 write`, `man 2 close`, `man 2 exit`,
`unistd_64.h`, `ausyscall`, `man 1 as`, `man 1 ld`.
