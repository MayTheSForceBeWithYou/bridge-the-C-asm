# 46 — nostdlib arena wordcount stretch (Track D)

## Goal
Stretch: freestanding program that `open`/`read`s a file into an mmap/bump
buffer, counts newlines, writes the count as decimal ASCII, exits — **zero
libc**.

## Do
1. `_start` in `wc_nl.s` (or split helpers). Open path from argv[1] or default
   `TASK.md`.
2. Read file into a buffer (mmap or .bss). Count `\n`.
3. Convert count to decimal ASCII without printf; `write` it + newline.
4. Verify: `nm ./wc_nl` and `readelf -d ./wc_nl` show no NEEDED libc.
5. `./wc_nl TASK.md` should print the newline count of this file.

## Success
Correct newline count; `nm`/`readelf` confirm no libc dependency.

## Refs
`man 2 open`, `man 2 read`, `man 2 write`, `man 2 close`, `man 2 mmap`,
`man 2 exit`, `man 1 nm`, `man 1 readelf`, `unistd_64.h`
