# 51 — _start to main (Track E)

## Goal
Disassemble `_start` and reason how control reaches `main` with `argc`/`argv`.

## Do
1. `make` then:
```
objdump -d -j .text ./prog | less
# find <_start>:
objdump -d ./prog | grep -A40 '<_start>:'
```
2. Trace by hand: which call goes toward `__libc_start_main` / main?
3. Docs: `info libc` if useful for startup; there is no man page for
   `__libc_start_main` here — reason from disassembly + info libc.
4. Optional gdb: `break _start`, `run`, `stepi` until `main`.

## Success
Written trace from `_start` to `main` citing your disassembly lines.

## Refs
`man 1 objdump`, `info libc`, `man 5 elf`, `man 1 gdb`
