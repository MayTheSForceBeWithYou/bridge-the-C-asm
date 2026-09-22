# 25 — `strlen`-like in asm (practice)

Read `LESSON.md` first — especially addressing-mode decoding and the empty-string
path. This file is only the lab.

## Build and run

1. Implement `long my_strlen(const char *s)` in `my_strlen.s` (load byte, test,
   increment, loop). Count bytes before NUL; `s` is non-NULL in tests.
2. Do not call libc `strlen`.
3. `make && make run`.

## Done when

- Harness prints `ok`.
- Disassembly of `<my_strlen>:` shows your byte loop, not `call strlen@plt`.

## Lookup

`man 1 as`, `info as`.
