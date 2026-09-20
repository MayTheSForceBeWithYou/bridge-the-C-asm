# 46 — nostdlib arena wordcount (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. Freestanding `_start` in `wc_nl.s` (helpers OK): open argv[1] or default `TASK.md`.
2. Read into mmap or `.bss`; count `\n`; write decimal ASCII + newline; exit.
3. Verify: `nm ./wc_nl` and `readelf -d ./wc_nl` show no NEEDED libc.
4. `./wc_nl TASK.md` prints this file's newline count.

## Done when

- Correct newline count; no libc dependency.

## Lookup

`man 2 open`, `man 2 read`, `man 2 write`, `man 2 close`, `man 2 mmap`, `man 2 exit`,
`man 1 nm`, `man 1 readelf`, `unistd_64.h`.
