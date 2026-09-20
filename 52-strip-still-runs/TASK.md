# 52 — strip still runs (practice)

Read `LESSON.md` first. This file is only the lab.

## Strip a copy

1. Implement hello; `make`; confirm `./prog` works.
2. `cp prog prog.stripped`; save `nm`/`size` before; `strip prog.stripped`; compare after;
   run `./prog.stripped`.
3. Contrast mentally with exercise 38 (gdb by address vs nm/size focus here).

## Done when

- Stripped binary runs; nm symbols largely gone; you noted the size change.

## Lookup

`man 1 strip`, `man 1 nm`, `man 1 size`, `info binutils`.
