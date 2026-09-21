# 51 — _start to main (practice)

Read `LESSON.md` first. This file is only the lab.

## Trace

1. `make`; `objdump -d ./prog | grep -A40 '<_start>:'` (and browse `.text`).
2. Hand-trace calls toward `__libc_start_main` / `main`.
3. Optional gdb: `break _start`, `stepi` until `main`.
4. Write the trace citing your disassembly lines (`info libc` OK; no fake man page).

## Done when

- Written trace from `_start` to `main` grounded in your dump.

## Lookup

`man 1 objdump`, `info libc`, `man 5 elf`, `man 1 gdb`.
