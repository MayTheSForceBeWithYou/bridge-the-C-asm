# 41 — asm mmap (Track D)

## Goal
`mmap` a page via raw syscall, write a pattern, read it back, `munmap`, exit.

Numbers: `mmap=9`, `munmap=11`, `exit=60`.
Common flags: `PROT_READ|PROT_WRITE=3`, `MAP_PRIVATE|MAP_ANONYMOUS=0x22`.

## Do
1. Implement `_start` in `mmap_demo.s`.
2. `mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)`.
3. Store a recognizable pattern (e.g. `0xDEADBEEF` at offset 0).
4. Read it back into a register; if mismatch, exit 1.
5. `munmap`; `exit 0`.
6. Optional strace: `command -v strace && strace -e mmap,munmap ./mmap_demo`.

## Success
`./mmap_demo` exits 0. You verified the pattern round-trip without libc.

## Refs
`man 2 mmap`, `man 2 munmap`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`
