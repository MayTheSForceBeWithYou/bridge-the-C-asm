# 22 — `add3` in asm (practice)

Read `LESSON.md` first — especially the argument/return register table and how
to find `<add3>:`. This file is only the lab.

## Build and run

1. Implement `long add3(long a, long b, long c)` in `add3.s` (stub currently
   returns garbage via `ret` only).
2. `make && make run` — harness expects `add3(1,2,3) == 6` and similar cases.
3. Optionally `objdump -d` the binary and confirm `<add3>:` leaves the sum in
   `%rax`.

## Done when

- Harness prints `ok` and exits 0 (failure prints `FAIL` and exits non-zero).

## Lookup

`man 1 as`, `man 1 gcc`, `info as`. ABI registers: your own ex. 04–05 `gcc -S`
output. Optional: nvim-dap into `add3` — see `NVIM_DAP.md`.
