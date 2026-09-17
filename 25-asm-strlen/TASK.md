# 25 — `strlen`-like in asm (Track B)

## Goal
Walk bytes until `\\0`.

## Contract
```c
long my_strlen(const char *s);  /* count bytes before NUL; s non-NULL */
```

## Do
1. Implement in `my_strlen.s` (load byte, test, increment, loop).
2. `make && make run`.

## Success
Harness prints `ok`.

## Refs
`man 1 as`, `info as`. Do not call libc `strlen`.
