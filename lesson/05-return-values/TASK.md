# 05 — Return values (practice)

Read `LESSON.md` first — especially **Scalar returns** and **Small struct
returns**. This file is only the lab.

## Build and inspect

1. Implement `ret_int`, `ret_long`, and `struct Pair ret_pair(void)` as described
   in the lesson.
2. Call them from `main` and print results. `make asm` at `-O0`.
3. For each, record the return channel from callee *and* caller.

## Reading the listings

- Search each `ret_*:` label; note the register (or pointer stores) at `ret`.
- Search `call ret_pair` in `main`; read the surrounding setup and unpack.

## Done when

- You named the register used at `ret` for `ret_int` / `ret_long`.
- You explained how `ret_pair` returns (registers, hidden pointer, or both) with
  evidence from both sides of the call.

## Lookup

`man 1 gcc`, `man 1 objdump` — discover from output.
