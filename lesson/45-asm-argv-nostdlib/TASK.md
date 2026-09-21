# 45 — argv from initial stack (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `_start` in `argv0.s`: load argc from `(%rsp)`, argv0 from `8(%rsp)`.
2. Compute argv0 length; `write(1, argv0, len)`; write a newline; `exit 0`.
3. Run `./argv0` and `./argv0 foo` — first printed token is argv[0].

## Done when

- Stdout starts with argv[0] text; freestanding (no libc).

## Lookup

`man 2 write`, `man 2 exit`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`.
