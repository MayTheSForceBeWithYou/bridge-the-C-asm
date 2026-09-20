# 41 — asm mmap (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `_start` in `mmap_demo.s`: `mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)`.
2. Numbers: `mmap=9`, `munmap=11`, `exit=60`; prot `3`; flags `0x22`.
3. Store a pattern (e.g. `0xDEADBEEF`); reload; mismatch → exit 1.
4. `munmap`; `exit 0`. Optional: `command -v strace && strace -e mmap,munmap ./mmap_demo`.

## Done when

- `./mmap_demo` exits 0 after a verified pattern round-trip.
- No libc in the freestanding binary.

## Lookup

`man 2 mmap`, `man 2 munmap`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`.
