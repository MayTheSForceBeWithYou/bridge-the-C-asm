# 46 — Freestanding wordcount stretch

This stretch combines Track D skills: parse argv, open/read a file, use mmap or BSS as
a buffer, count newlines, and print a decimal integer — all without libc. It is a
checkpoint that you can assemble a tiny useful tool from syscalls alone.

## What this lesson asks of you

Build `wc_nl` freestanding: open a path (argv[1] or default `TASK.md`), count `\n`
bytes, write the count as ASCII decimal plus newline, exit. `nm`/`readelf -d` must show
no NEEDED libc.

## Pieces you already know

- **argv** (exercise 45): path string from the stack.
- **open/read/close/write/exit** (40): move file bytes.
- **mmap** (41) or a large `.bss` buffer: hold contents (or stream and count without
  storing everything — either design works if the count is correct).
- **No printf**: convert an integer to decimal by repeated division, emitting digits into
  a small buffer (reverse digit order carefully).

`readelf -d` on a dynamic-looking binary would show `NEEDED`; a true freestanding
`ld`-linked binary should lack an interpreter/NEEDED libc. Confirm with `nm` that you
did not accidentally link `-lc`.

## Decimal without libc

For count `n`, repeatedly `n % 10` / `n / 10` into a temporary buffer from the end, then
`write` the digit substring. Handle `n == 0` as a single `'0'`. Newline after the number.

## Worked example

**The situation.** `./wc_nl TASK.md` prints a number matching `grep -c` style newline
count for that file (count of `\n` bytes). `readelf -d` shows no libc NEEDED.

**Step 1 — open the path.** From argv or default. Failure exits non-zero. The rejected
wrong reading is: "I can call `atoi`/`printf` just for the count." That pulls libc and
fails the stretch goal.

**Step 2 — count.** Either map/read whole file or loop `read` and tally. The rejected
wrong reading is: "count characters that look like words." This tool counts newline
bytes only.

**Step 3 — emit decimal.** Write digits + `\n`. Verify against a known file. Confirm
freestanding with `nm`/`readelf`.

## Distinctions worth keeping straight

- **Streaming count vs full buffer** — both valid; mmap is convenient, not mandatory.
- **Newline count vs `wc -w` words** — different problems.
- **Dynamic libc hello vs nostdlib tool** — NEEDED entries tell the story.
- **Digit conversion endianness of emission** — build reversed, then write forward.

## Check yourself

1. How do you prove the binary has no libc dependency?
2. What is the output format required (digits + newline)?
3. Where does the pathname come from if argv[1] is absent?
4. Why is `printf("%d\n", n)` incompatible with the success criteria?

## Key takeaways

- Syscall-only programs can still do useful I/O and formatting.
- Combining argv, file I/O, and integer rendering is the Track D integration test.
- Tooling (`nm`, `readelf -d`) verifies freestanding linkage.
- Correctness is a counted `\n` and an honest decimal print.

## Lookup (not the lesson)

- `man 2 open`, `man 2 read`, `man 2 write`, `man 2 close`, `man 2 mmap`, `man 2 exit`
- `man 1 nm`, `man 1 readelf`, `unistd_64.h`

Now open `TASK.md` and do the practice.
